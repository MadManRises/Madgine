#pragma once

#include "../forward_capture.h"
#include "../pipable.h"
#include "sender.h"

#include "../tag_invoke.h"

#include "../projections.h"
#include "../type_pack.h"

namespace Engine {
namespace Execution {

    template <typename S>
    struct base_receiver {
        template <typename... V>
        void set_value(V... value)
        {
            mState->set_value(value...);
        }

        void set_done()
        {
            mState->set_done();
        }

        template <typename... R>
        void set_error(R... result)
        {
            mState->set_error(result...);
        }

        S *mState;



        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, base_receiver &rec)
        {
            return f(*rec.mState);
        }
    };

    template <typename Rec>
    struct base_state {
        template <typename... V>
        void set_value(V... value)
        {
            mRec.set_value(value...);
        }
        void set_done()
        {
            mRec.set_done();
        }

        template <typename... R>
        void set_error(R... result)
        {
            mRec.set_error(result...);
        }

        Rec mRec;



        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, base_state &state)
        {
            return f(state.mRec);
        }
    };

    template <typename Self, typename Rec, typename F>
    struct algorithm_state : base_state<Rec> {
        using receiver = base_receiver<Self>;

        algorithm_state(Rec &&rec, F &&sender)
            : base_state<Rec> { std::forward<Rec>(rec) }
            , mState { sender(receiver { static_cast<Self *>(this) }) }
        {
        }

        void start()
        {
            mState.start();
        }

        std::invoke_result_t<F, receiver> mState;
    };

    struct just_t {

        template <typename Rec, typename... Args>
        struct state : base_state<Rec> {
            void start()
            {
                TupleUnpacker::invokeFromTuple(LIFT(mRec.set_value, this), mArgs);
            }

            std::tuple<Args...> mArgs;
        };

        template <typename... Args>
        auto operator()(Args &&...args) const
        {
            return make_sender<void, Args...>(
                [args { std::tuple<Args...> { std::forward<Args>(args)... } }]<typename Rec>(Rec &&rec) {
                    return state<Rec, Args...> { std::forward<Rec>(rec), std::move(args) };
                });
        }
    };

    inline constexpr just_t just;

    struct then_t {

        template <typename T, typename Rec, typename F, typename R, typename... V>
        struct state : algorithm_state<state<T, Rec, F, R, V...>, Rec, F> {
            state(Rec &&rec, Sender<F, R, V...> &&sender, T &&transform)
                : algorithm_state<state<T, Rec, F, R, V...>, Rec, F> { std::forward<Rec>(rec), std::move(sender) }
                , mTransform(std::forward<T>(transform))
            {
            }

            void set_value(V... values)
            {
                using V2 = std::invoke_result_t<T, V...>;
                if constexpr (std::same_as<V2, void>) {
                    mTransform(std::forward<V>(values)...);
                    mRec.set_value();
                } else
                    mRec.set_value(mTransform(std::forward<V>(values)...));
            }

            T mTransform;
        };

        template <typename F, typename R, typename... V, typename T>
        friend auto tag_invoke(then_t, Sender<F, R, V...> &&sender, T &&transform)
        {
            using V2 = std::invoke_result_t<T, V...>;
            return make_sender<R, V2>(
                [sender { std::move(sender) }, transform { forward_capture(std::forward<T>(transform)) }]<typename Rec>(Rec &&rec) mutable {
                    return state<T, Rec, F, R, V...> { std::forward<Rec>(rec), std::move(sender), std::forward<T>(transform) };
                });
        }

        template <typename S, typename T>
        requires tag_invocable<then_t, S, T>
        auto operator()(S &&sender, T &&transform) const
            noexcept(is_nothrow_tag_invocable_v<then_t, S, T>)
                -> tag_invoke_result_t<then_t, S, T>
        {
            return tag_invoke(*this, std::forward<S>(sender), std::forward<T>(transform));
        }

        template <typename T>
        auto operator()(T &&transform) const
        {
            return pipable_from_right(*this, std::forward<T>(transform));
        }
    };

    inline constexpr then_t then;

    template <typename Rec1, typename Rec2, typename R, typename... V>
    struct CombinedRec {
        void set_value(V... value)
        {
            mRec1.set_value(value...);
            mRec2.set_value(std::forward<V>(value)...);
        }
        void set_done()
        {
            mRec1.set_done();
            mRec2.set_done();
        }
        void set_error(R result)
        {
            mRec1.set_error(result);
            mRec2.set_error(std::forward<R>(result));
        }
        Rec1 mRec1;
        Rec2 mRec2;
    };

    struct then_receiver_t {
        template <typename F, typename R, typename... V, typename Rec1>
        friend auto tag_invoke(then_receiver_t, Sender<F, R, V...> &&sender, Rec1 &&rec1)
        {
            return make_sender<R, V...>(
                [sender { std::move(sender) }, rec1 { forward_capture(std::forward<Rec1>(rec1)) }]<typename Rec2>(Rec2 &&rec2) mutable {
                    return sender(CombinedRec<Rec1, Rec2, R, V...> { std::forward<Rec1>(rec1), std::forward<Rec2>(rec2) });
                });
        }

        template <typename S, typename Rec1>
        requires tag_invocable<then_receiver_t, S, Rec1>
        auto operator()(S &&sender, Rec1 &&rec1) const
            noexcept(is_nothrow_tag_invocable_v<then_receiver_t, S, Rec1>)
                -> tag_invoke_result_t<then_receiver_t, S, Rec1>
        {
            return tag_invoke(*this, std::forward<S>(sender), std::forward<Rec1>(rec1));
        }

        template <typename Rec1>
        auto operator()(Rec1 &&rec1) const
        {
            return pipable_from_right(*this, std::forward<Rec1>(rec1));
        }
    };

    inline constexpr then_receiver_t then_receiver;

    struct for_each_t {

        template <typename Rec, typename C, typename F>
        struct state : base_state<Rec> {
            using value_type = typename std::remove_reference_t<C>::value_type &;
            using inner_sender_t = std::invoke_result_t<F, value_type>;
            using receiver_t = base_receiver<state>;
            using inner_state_t = std::invoke_result_t<inner_sender_t, receiver_t>;

            state(Rec &&rec, C &&c, F &&f)
                : base_state<Rec> { std::forward<Rec>(rec) }
                , mF(std::forward<F>(f))
                , mC(std::forward<C>(c))
            {
            }

            ~state() { }

            void start()
            {
                mIt = mC.begin();
                loop();
            }

            void stopInner()
            {
                mState.~inner_state_t();
            }

            void set_value()
            {
                stopInner();
                if (mFlag.test_and_set())
                    loop();
            }

            void set_done()
            {
                stopInner();
                mRec.set_done();
            }

            template <typename... R>
            void set_error(R... result)
            {
                stopInner();
                mRec.set_error(result...);
            }

            void loop()
            {
                while (proceed())
                    ++mIt;
            }

            bool proceed()
            {
                if (mIt == mC.end()) {
                    mRec.set_value();
                    return false;
                } else {
                    mFlag.clear();
                    new (&mState) inner_state_t { mF(*mIt)(receiver_t { this }) };
                    mState.start();
                    return mFlag.test_and_set();
                }
            }

            F mF;
            using iterator = typename std::remove_reference_t<C>::iterator;
            C mC;
            iterator mIt;
            std::atomic_flag mFlag;
            union {
                inner_state_t mState;
            };
        };

        template <typename C, typename F>
        friend auto tag_invoke(for_each_t, C &&c, F &&f)
        {
            using R = std::invoke_result_t<F, std::remove_reference_t<C>::value_type>::result_type;
            return make_sender<R>(
                [c { forward_capture(std::forward<C>(c)) }, f { forward_capture(std::forward<F>(f)) }]<typename Rec>(Rec &&rec) mutable {
                    return state<Rec, C, F> { std::forward<Rec>(rec), std::forward<C>(c), std::forward<F>(f) };
                });
        }

        template <typename C, typename F>
        requires tag_invocable<for_each_t, C, F>
        auto operator()(C &&container, F &&f) const
            noexcept(is_nothrow_tag_invocable_v<for_each_t, C, F>)
                -> tag_invoke_result_t<for_each_t, C, F>
        {
            return tag_invoke(*this, std::forward<C>(container), std::forward<F>(f));
        }
    };

    inline constexpr for_each_t for_each;

    struct repeat_t {

        template <typename Rec, typename F, typename R, typename... V>
        struct state : algorithm_state<state<Rec, F, R, V...>, Rec, F> {
            state(Rec rec, Sender<F, R, V...> &&sender)
                : algorithm_state<state<Rec, F, R, V...>, Rec, F>(std::forward<Rec>(rec), std::move(sender))
            {
            }

            void set_value(V... args)
            {
                start();
            }
            void set_error(R result)
            {
                mRec.set_error(result);
            }
            void set_done()
            {
                mRec.set_done();
            }
        };

        template <typename F, typename R, typename... V>
        friend auto tag_invoke(repeat_t, Sender<F, R, V...> &&sender)
        {
            return make_sender<R>(
                [sender { std::move(sender) }]<typename Rec>(Rec &&rec) mutable {
                    return state<Rec, F, R, V...> { std::forward<Rec>(rec), std::move(sender) };
                });
        }

        template <typename S>
        requires tag_invocable<repeat_t, S>
        auto operator()(S &&sender) const
            noexcept(is_nothrow_tag_invocable_v<repeat_t, S>)
                -> tag_invoke_result_t<repeat_t, S>
        {
            return tag_invoke(*this, std::forward<S>(sender));
        }

        template <typename S>
        friend auto operator|(S &&sender, repeat_t r)
        {
            return tag_invoke(r, std::forward<S>(sender));
        }
    };

    inline constexpr repeat_t repeat;

    struct when_all_t {

        template <size_t I, typename S, typename V>
        struct inner_receiver : base_receiver<S> {
            void set_value(V value)
            {
                mState->template set_value<I>(value);
            }
        };

        template <size_t I, typename S, typename F, typename V>
        struct inner_state {
            inner_state(auto &&ctor)
                : mState { ctor() }
            {
            }

            void start()
            {
                mState.start();
            }

            std::invoke_result_t<F, inner_receiver<I, S, V>> mState;
        };

        template <typename Rec, typename Is, typename... S>
        struct state;

        template <typename Rec, typename R, typename... F, typename... V, size_t... Is>
        struct state<Rec, std::index_sequence<Is...>, Sender<F, R, V>...> {
            state(Rec &&rec, Sender<F, R, V> &&...senders)
                : mRec(std::forward<Rec>(rec))
                , mStates { [&]() { return senders(inner_receiver<Is, state, V> { this }); }... }
            {
            }

            void start()
            {
                mCompleteCount = 0;
                mState = OK;
                TupleUnpacker::forEach(mStates, [](auto &state) { state.start(); });
                mark_complete();
            }

            template <size_t I>
            void set_value(typename type_pack<V...>::select<I> value)
            {
                if (mState == OK)
                    std::get<I>(mValues) = value;
                mark_complete();
            }

            void set_done()
            {
                mState = DONE;
                mark_complete();
            }

            void set_error(patch_void_t<R> result)
            {
                State expected = OK;
                if (mState.compare_exchange_strong(expected, ERROR)) {
                    mResult = result;
                }
                mark_complete();
            }

            void mark_complete()
            {
                size_t count = mCompleteCount.fetch_add(1);
                if (count == sizeof...(Is)) {
                    switch (mState) {
                    case OK:
                        TupleUnpacker::invokeFromTuple(LIFT(mRec.set_value, this), mValues);
                        break;
                    case ERROR:
                        mRec.set_error(mResult);
                        break;
                    case DONE:
                        mRec.set_done();
                        break;
                    default:
                        throw 0;
                    }
                }
            }

            Rec mRec;
            std::tuple<inner_state<Is, state, F, V>...> mStates;
            patch_void_t<R> mResult;
            std::tuple<std::conditional_t<std::is_reference_v<V>, OutRef<std::remove_reference_t<V>>, V>...> mValues;
            enum State { OK,
                ERROR,
                DONE };
            std::atomic<State> mState = OK;
            std::atomic<size_t> mCompleteCount = 0;
        };

        template <typename R, typename... F, typename... V>
        friend auto tag_invoke(when_all_t, Sender<F, R, V> &&...senders)
        {
            return make_sender<R, V...>(
                [senders { std::make_tuple(std::move(senders)...) }]<typename Rec>(Rec &&rec) mutable {
                    return TupleUnpacker::constructExpand<state<Rec, std::index_sequence_for<F...>, Sender<F, R, V>...>>(std::forward<Rec>(rec), std::move(senders));
                });
        }

        template <typename... S>
        requires tag_invocable<when_all_t, S...>
        auto operator()(S &&...senders) const
            noexcept(is_nothrow_tag_invocable_v<when_all_t, S...>)
                -> tag_invoke_result_t<when_all_t, S...>
        {
            return tag_invoke(*this, std::forward<S>(senders)...);
        }
    };

    inline constexpr when_all_t when_all;

    struct sequence_t {

        template <typename Rec, typename R, typename F1, typename F2, typename... V1>
        struct state : algorithm_state<state<Rec, R, F1, F2, V1...>, Rec, F1> {
            state(Rec &&rec, F1 &&sender1, F2 &&sender2)
                : algorithm_state<state<Rec, R, F1, F2, V1...>, Rec, F1> { std::forward<Rec>(rec), std::forward<F1>(sender1) }
                , mSecondState { sender2(mRec) }
            {
            }

            void set_value(V1... values)
            {
                mSecondState.start();
            }

            std::invoke_result_t<F2, Rec &> mSecondState;
        };

        template <typename R, typename F, typename... V>
        friend auto tag_invoke(sequence_t, Sender<F, R, V...> &&sender)
        {
            return std::move(sender);
        }

        template <typename R, typename F1, typename F2, typename... V1, typename... V2, typename... S>
        friend auto tag_invoke(sequence_t sequence, Sender<F1, R, V1...> &&sender1, Sender<F2, R, V2...> &&sender2, S &&...senders)
        {
            return sequence(
                make_sender<R, V2...>(
                    [sender1 { std::move(sender1) }, sender2 { std::move(sender2) }]<typename Rec>(Rec &&rec) mutable {
                        return state<Rec, R, F1, F2, V1...> { std::forward<Rec>(rec), std::move(sender1), std::move(sender2) };
                    }),
                std::forward<S>(senders)...);
        }

        template <typename... S>
        requires tag_invocable<sequence_t, S...>
        auto operator()(S &&...senders) const
            noexcept(is_nothrow_tag_invocable_v<sequence_t, S...>)
                -> tag_invoke_result_t<sequence_t, S...>
        {
            return tag_invoke(*this, std::forward<S>(senders)...);
        }
    };

    inline constexpr sequence_t sequence;

    struct let_value_t {

        template <typename Rec, typename F2, typename F, typename R, typename... V>
        struct state : algorithm_state<state<Rec, F2, F, R, V...>, Rec, F> {
            using inner_sender_t = std::invoke_result_t<F2, V...>;
            using inner_state = std::invoke_result_t<inner_sender_t, Rec &>;

            state(Rec &&rec, Sender<F, R, V...> &&sender, F2 &&f)
                : algorithm_state<state<Rec, F2, F, R, V...>, Rec, F> { std::forward<Rec>(rec), std::move(sender) }
                , mF(std::forward<F2>(f))
            {
            }

            void set_value(V... values)
            {
                new (&mValue) std::tuple<V...> { { values... } };
                new (&mInnerState) inner_state { TupleUnpacker::invokeFromTuple(mF, mValue)(mRec) };
                mInnerState.start();
            }

            F2 mF;
            union {
                struct {
                    inner_state mInnerState;
                    std::tuple<V...> mValue;
                };
            };
        };

        template <typename F2, typename F, typename R, typename... V>
        friend auto tag_invoke(let_value_t, Sender<F, R, V...> &&sender, F2 &&f)
        {
            using R2 = std::invoke_result_t<F2, V...>::result_type;
            return make_sender<R2>(
                [sender { std::move(sender) }, f { forward_capture(std::forward<F2>(f)) }]<typename Rec>(Rec &&rec) mutable {
                    return state<Rec, F2, F, R, V...> { std::forward<Rec>(rec), std::move(sender), std::forward<F2>(f) };
                });
        }

        template <typename S, typename F2>
        requires tag_invocable<let_value_t, S, F2>
        auto operator()(S &&sender, F2 &&f) const
            noexcept(is_nothrow_tag_invocable_v<let_value_t, S, F2>)
                -> tag_invoke_result_t<let_value_t, S, F2>
        {
            return tag_invoke(*this, std::forward<S>(sender), std::forward<F2>(f));
        }

        template <typename F2>
        auto operator()(F2 &&f) const
        {
            return pipable_from_right(*this, std::forward<F2>(f));
        }
    };

    inline constexpr let_value_t let_value;
}
}