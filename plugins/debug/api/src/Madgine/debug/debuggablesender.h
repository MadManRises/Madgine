#pragma once

#include "continuation.h"
#include "senderlocation.h"

namespace Engine {
namespace Execution {

    struct get_debug_location_t {

        using signature = Debug::SenderLocation *();

        template <typename T>
            requires(!tag_invocable<get_debug_location_t, T &>)
        auto operator()(T &t) const
        {
            return nullptr;
        }

        template <typename T>
            requires tag_invocable<get_debug_location_t, T &>
        auto operator()(T &t) const
            noexcept(is_nothrow_tag_invocable_v<get_debug_location_t, T &>)
                -> tag_invoke_result_t<get_debug_location_t, T &>
        {
            return tag_invoke(*this, t);
        }
    };

    inline constexpr get_debug_location_t get_debug_location;

    struct with_debug_location_t {

        template <typename Sender, typename _Rec>
        struct state;

        template <typename Sender, typename Rec>
        struct receiver {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done();
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(std::forward<R>(result)...);
            }

            friend Debug::SenderLocation *tag_invoke(get_debug_location_t, receiver &rec)
            {
                return &rec.mState->mLocation;
            }

            template <typename CPO, typename... Args>
                requires(is_tag_invocable_v<CPO, Rec &, Args...>)
            friend auto tag_invoke(CPO f, receiver &rec, Args &&...args) noexcept(is_nothrow_tag_invocable_v<CPO, Rec &, Args...>)
                -> tag_invoke_result_t<CPO, Rec &, Args...>
            {
                return tag_invoke(f, rec.mState->mRec, std::forward<Args>(args)...);
            }

            state<Sender, Rec> *mState;
        };

        template <typename Sender, typename _Rec>
        struct state {
            using InnerRec = _Rec;
            using Rec = receiver<Sender, InnerRec>;

            using State = connect_result_t<Sender, Rec>;

            state(Sender &&sender, InnerRec &&rec)
                : mRec(std::forward<InnerRec>(rec))
                , mLocation([this](Debug::SenderLocation::CB visitor) { visit_state(&mState, std::move(visitor)); })
                , mState { connect(std::forward<Sender>(sender), Rec { this }) }
            {
            }

            ~state() { }

            void start()
            {
                Debug::stepInto(mRec, &mLocation);
                mState.start();
            }

            void stop()
            {
                mState.stop();
            }

            template <typename... V>
            void set_value(V &&...value)
            {
                Debug::stepOut(mRec, &mLocation);
                mRec.set_value(std::forward<V>(value)...);
            }

            void set_done()
            {
                Debug::stepOut(mRec, &mLocation);
                mRec.set_done();
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                Debug::stepOut(mRec, &mLocation);
                mRec.set_error(std::forward<R>(result)...);
            }

            /* friend auto tag_invoke(Execution::visit_state_t, state &state, const auto &info, auto &&visitor, bool running)
            {
                Execution::visit_state(state.mState, info, std::forward<decltype(visitor)>(visitor), running);
            }*/

            InnerRec mRec;
            Debug::SenderLocation mLocation;
            State mState;
        };

        template <AnySender Sender>
        struct sender : algorithm_sender<Sender> {

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Sender, Rec> { std::forward<Sender>(sender.mSender), std::forward<Rec>(rec) };
            }
        };

        template <AnySender Sender>
        friend auto tag_invoke(with_debug_location_t, Sender &&inner)
        {
            return sender<Sender> { { {}, std::forward<Sender>(inner) } };
        }

        template <AnySender Sender>
            requires tag_invocable<with_debug_location_t, Sender>
        auto operator()(Sender &&sender) const
            noexcept(is_nothrow_tag_invocable_v<with_debug_location_t, Sender>)
                -> tag_invoke_result_t<with_debug_location_t, Sender>
        {
            return tag_invoke(*this, std::forward<Sender>(sender));
        }

        auto operator()() const
        {
            return pipable_from_right(*this);
        }
    };

    inline constexpr with_debug_location_t with_debug_location;

    template <typename T>
    concept is_debuggable = std::same_as<tag_invoke_result_t<get_debug_location_t, T &>, Debug::SenderLocation *>;

    struct tracked_t {

        template <typename Sender, is_debuggable Rec>
        struct state;

        template <typename Sender, is_debuggable Rec>
        struct receiver {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState.mPausedAtStart = false;

                Debug::SenderLocation *location = get_debug_location(this->mState.mRec);

                mState.mContinuation.returnValue(location, this->mState.mRec, mState.mEndBreakpoint, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState.mPausedAtStart = false;

                Debug::SenderLocation *location = get_debug_location(this->mState.mRec);

                mState.mContinuation.returnDone(location, this->mState.mRec, mState.mEndBreakpoint);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState.mPausedAtStart = false;

                Debug::SenderLocation *location = get_debug_location(this->mState.mRec);

                mState.mContinuation.returnError(location, this->mState.mRec, mState.mEndBreakpoint, std::forward<R>(result)...);
            }

            friend Debug::Continuation *tag_invoke(Debug::get_continuation_t, receiver &rec)
            {
                return &rec.mState.mContinuation;
            }

            template <typename CPO, typename... Args>
            friend auto tag_invoke(CPO f, receiver &rec, Args &&...args)
                -> tag_invoke_result_t<CPO, Rec &, Args...>
            {
                return f(rec.mState.mRec, std::forward<Args>(args)...);
            }

            state<Sender, Rec> &mState;
        };

        template <typename Sender, is_debuggable Rec>
        struct state {

            using State = tag_invoke_result_t<connect_t, Sender, receiver<Sender, Rec>>;

            state(Rec &&rec, Sender &&sender)
                : mRec(std::forward<Rec>(rec))
                , mState { tag_invoke(connect_t {}, std::forward<Sender>(sender), receiver<Sender, Rec> { *this }) }
            {
            }

            ~state() { }

            void start()
            {
                mPausedAtStart = true;

                Debug::SenderLocation *location = get_debug_location(mRec);

                mContinuation.start(mRec);

                mContinuation.pass(location, mRec, [this](Rec &rec) { mState.start(); }, mStartBreakpoint);
            }

            void stop()
            {
                if (mContinuation.stop()) {
                    mState.stop();
                    mContinuation.done();
                }
            }

            friend auto tag_invoke(visit_state_t, state *state, auto &&visitor)
            {
                bool pausedAtStart = state && state->mPausedAtStart;

                visitor(Execution::State::Breakpoint {
                    state ? &state->mStartBreakpoint : nullptr,
                    state ? &state->mContinuation : nullptr,
                    [state]() { return Closure<void()> { [state]() { state->mState.stop(); } }; },
                    pausedAtStart });
                visit_state(state ? &state->mState : nullptr, std::forward<decltype(visitor)>(visitor));
                visitor(Execution::State::Breakpoint {
                    state ? &state->mEndBreakpoint : nullptr,
                    state ? &state->mContinuation : nullptr,
                    [state]() { return Closure<void()> { [state]() { state->mState.stop(); } }; },
                    !pausedAtStart });
            }

            Rec mRec;
            State mState;

            IndexType<size_t> mStartBreakpoint;
            IndexType<size_t> mEndBreakpoint;
            bool mPausedAtStart;
            Debug::Continuation mContinuation;
        };

        template <AnySender Sender>
        struct sender : algorithm_sender<Sender> {

            template <is_debuggable Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Sender, Rec> { std::forward<Rec>(rec), std::forward<Sender>(sender.mSender) };
            }

            template <is_debuggable Rec>
            friend auto tag_invoke(connect_t, sender &sender, Rec &&rec)
            {
                return state<Sender &, Rec> { std::forward<Rec>(rec), sender.mSender };
            }
        };

        template <typename Sender>
        friend auto tag_invoke(tracked_t, Sender &&inner)
        {
            return sender<Sender> { { {}, std::forward<Sender>(inner) } };
        }

        template <typename Sender>
            requires tag_invocable<tracked_t, Sender>
        auto operator()(Sender &&sender) const
            noexcept(is_nothrow_tag_invocable_v<tracked_t, Sender>)
                -> tag_invoke_result_t<tracked_t, Sender>
        {
            return tag_invoke(*this, std::forward<Sender>(sender));
        }
    };

    inline constexpr tracked_t tracked;

    template <typename Sender>
    using wrap = tracked_t::sender<Sender>;

    template <typename Sender, is_debuggable Rec>
        requires(tag_invocable<connect_t, wrap<Sender>, Rec>)
    auto tag_invoke(outer_connect_t, Sender &&sender, Rec &&rec) noexcept(is_nothrow_tag_invocable_v<connect_t, wrap<Sender>, Rec>)
        -> tag_invoke_result_t<connect_t, wrap<Sender>, Rec>
    {
        return tag_invoke(connect_t {}, tracked(std::forward<Sender>(sender)), std::forward<Rec>(rec));
    }

}
}