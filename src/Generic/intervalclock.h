#pragma once

#include "execution/concepts.h"
#include "execution/statedescriptor.h"
#include "genericresult.h"

#include "execution/stop_callback.h"

#include "execution/algorithm.h"

namespace Engine {

struct get_behavior_name_t;

template <typename Timepoint = std::chrono::steady_clock::time_point>
struct IntervalClock {

    IntervalClock(Timepoint start)
        : mLastTick(start)
    {
    }

    std::chrono::microseconds tick(Timepoint now)
    {
        std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - mLastTick);
        mLastTick = now;

        if (elapsed.count() > 0) {
            std::vector<WaitState *> doneWaits;
            {
                std::lock_guard guard { mMutex };
                for (auto it = mWaitStates.begin(); it != mWaitStates.end();) {
                    if ((*it)->mWaitUntil <= mLastTick) {
                        doneWaits.push_back(*it);
                        it = mWaitStates.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            for (WaitState *waitState : doneWaits)
                waitState->continueExecution(elapsed);
        }

        return elapsed;
    }

    struct WaitState {

        WaitState(std::chrono::steady_clock::duration duration)
            : mDuration(duration)
        {
        }

        virtual void start(IntervalClock *clock)
        {
            mClock = clock;
            mWaitUntil = mClock->mLastTick + mDuration;
            std::lock_guard guard { mClock->mMutex };
            mClock->mWaitStates.push_back(this);
        }

        void cancel()
        {
            std::lock_guard guard { mClock->mMutex };
            std::erase(mClock->mWaitStates, this);
        }

        template <typename F>
        friend void tag_invoke(Execution::visit_state_t, WaitState &state, F &&f)
        {
            if (state.mDuration.count() > 0) {
                std::chrono::steady_clock::duration elapsed = std::chrono::duration_cast<std::chrono::steady_clock::duration>(state.mWaitUntil - state.mClock->mLastTick);
                f(Execution::State::Progress { 1.0f - (static_cast<float>(elapsed.count()) / state.mDuration.count()) });
            }
        }

        friend std::string tag_invoke(get_behavior_name_t, const WaitState &state)
        {
            std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds>(state.mDuration);
            if (dur.count() == 0) {
                return "Yield";
            } else if (dur.count() < 1000) {
                return std::format("Waiting {} ns", dur.count());
            } else if (dur.count() < 1000000) {
                return std::format("Waiting {:.3f} us", std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(dur).count());
            } else if (dur.count() < 1000000000) {
                return std::format("Waiting {:.4f} ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
            } else {
                return std::format("Waiting {:.4f} s", std::chrono::duration_cast<std::chrono::duration<float>>(dur).count());
            }
        }

        virtual void continueExecution(std::chrono::microseconds elapsed) = 0;

        IntervalClock *mClock = nullptr;
        std::chrono::steady_clock::duration mDuration;
        Timepoint mWaitUntil;
    };

    template <typename Rec>
    struct receiver : Execution::algorithm_receiver<Rec> {

        receiver(Rec &&rec, WaitState *state)
            : Execution::algorithm_receiver<Rec> { std::forward<Rec>(rec) }
            , mState(state)
        {
        }

        void set_value(IntervalClock *clock)
        {
            mState->start(clock);
        }

        void set_value(IntervalClock &clock)
        {
            set_value(&clock);
        }

        WaitState *mState;
    };

    template <typename Inner, typename Rec>
    struct state : WaitState, Execution::base_state<Rec> {

        state(Inner &&inner, Rec &&rec, std::chrono::steady_clock::duration duration)
            : WaitState(duration)
            , Execution::base_state<Rec>(std::forward<Rec>(rec))
            , mInnerState(Execution::connect(std::forward<Inner>(inner), receiver<Rec &> { mRec, this }))
            , mCallback(cleanup_cb { mRec })
        {
        }

        void start()
        {
            mInnerState.start();
        }

        virtual void start(IntervalClock *clock) override
        {
            std::stop_token st = Execution::get_stop_token(mRec);
            if (st.stop_requested()) {
                mRec.set_done();
            } else {
                WaitState::start(clock);
                mCallback.start(std::move(st), stop_cb { this });
            }
        }

        virtual void continueExecution(std::chrono::microseconds elapsed) override
        {
            mCallback.finish(std::move(elapsed));
        }

        struct stop_cb {
            void operator()()
            {
                mState->cancel();
            }

            WaitState *mState;
        };

        struct cleanup_cb {
            void operator()(std::chrono::microseconds elapsed)
            {
                if (Execution::get_stop_token(mRec).stop_requested())
                    mRec.set_done();
                else
                    mRec.set_value(std::move(elapsed));
            }

            void operator()(Execution::cancelled_t)
            {
                assert(Execution::get_stop_token(mRec).stop_requested());
                mRec.set_done();
            }

            Rec &mRec;
        };

        Execution::connect_result_t<Inner, receiver<Rec &>> mInnerState;
        Execution::stop_callback<stop_cb, cleanup_cb> mCallback;
    };

    template <typename Inner>
    struct sender : Execution::algorithm_sender<Inner> {

        using result_type = GenericResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<std::chrono::microseconds>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
        {
            return state<Inner, Rec> { std::forward<Inner>(sender.mInner), std::forward<Rec>(rec), sender.mDuration };
        }

        Inner mInner;
        std::chrono::steady_clock::duration mDuration;
    };

    /* auto wait(Timepoint until)
    {
        return sender { {}, this, until };
    }*/

    auto wait(std::chrono::steady_clock::duration duration)
    {
        return sender<Execution::just_t::sender<IntervalClock *>> { {}, Execution::just(this), duration };
    }

    template <typename Sender>
    static auto wait(Sender &&inner, std::chrono::steady_clock::duration duration)
    {
        return sender<Sender> { {}, std::forward<Sender>(inner), duration };
    }

private:
    Timepoint mLastTick;

    std::mutex mMutex;
    std::vector<WaitState *> mWaitStates;
};

}