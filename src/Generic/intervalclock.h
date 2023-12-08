#pragma once

#include "execution/concepts.h"
#include "execution/statedescriptor.h"
#include "genericresult.h"

#include "execution/stop_callback.h"

namespace Engine {

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

        WaitState(IntervalClock *clock, Timepoint waitUntil)
            : mClock(clock)
            , mWaitUntil(waitUntil)
        {
        }

        void start()
        {
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
            f(Execution::State::Text { "Waiting " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(state.mWaitUntil - state.mClock->mLastTick).count()) + " ms" });
        }

        virtual void continueExecution(std::chrono::microseconds elapsed) = 0;

        IntervalClock *mClock;
        Timepoint mWaitUntil;
    };

    template <typename _Rec>
    struct state : WaitState {

        using Rec = _Rec;

        state(Rec &&rec, IntervalClock *clock, Timepoint waitUntil)
            : WaitState(clock, waitUntil)
            , mRec(std::forward<Rec>(rec))
            , mCallback(cleanup_cb {this})
        {
        }

        void start() {
            std::stop_token st = Execution::get_stop_token(mRec);
            if (st.stop_requested()) {
                mRec.set_done();
            } else {
                WaitState::start();
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
                if (Execution::get_stop_token(mState->mRec).stop_requested())
                    mState->mRec.set_done();
                else
                    mState->mRec.set_value(std::move(elapsed));
            }

            void operator()(Execution::cancelled_t)
            {
                assert(Execution::get_stop_token(mState->mRec).stop_requested());
                mState->mRec.set_done();
            }

            state *mState;
        };

        Rec mRec;
        Execution::stop_callback<stop_cb, cleanup_cb> mCallback;
    };

    struct sender : Execution::base_sender {

        using result_type = GenericResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<std::chrono::microseconds>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
        {
            return state<Rec> { std::forward<Rec>(rec), sender.mClock, sender.mWaitUntil };
        }

        IntervalClock *mClock;
        Timepoint mWaitUntil;
    };

    auto wait(Timepoint until)
    {
        return sender { {}, this, until };
    }

    auto wait(std::chrono::steady_clock::duration duration)
    {
        return wait(mLastTick + duration);
    }

private:
    Timepoint mLastTick;

    std::mutex mMutex;
    std::vector<WaitState *> mWaitStates;
};

}