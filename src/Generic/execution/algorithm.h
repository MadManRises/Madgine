#pragma once

#include "../pipable.h"
#include "../type_pack.h"
#include "concepts.h"

namespace Engine {
namespace Execution {

    struct just_t {

        template <typename Rec, typename... Args>
        struct state : base_state<Rec> {

            void start()
            {
                TupleUnpacker::invokeFromTuple(LIFT(this->mRec.set_value, this), mArgs);
            }

            std::tuple<Args...> mArgs;
        };

        template <typename... Args>
        struct sender {
            using result_type = void;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<Args...>;

            using is_sender = void;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Args...> { std::forward<Rec>(rec), std::move(sender.mArgs) };
            }

            std::tuple<Args...> mArgs;
        };

        template <typename... Args>
        auto operator()(Args &&...args) const
        {
            return sender<std::decay_t<Args>...> { { std::forward<Args>(args)... } };
        }
    };

    inline constexpr just_t just;

    template <typename... T>
    struct just_error_t {

        template <typename Rec, typename R>
        struct state : base_state<Rec> {

            void start()
            {
                this->mRec.set_error(mError);
            }

            R mError;
        };

        template <typename R>
        struct sender {
            using result_type = R;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<T...>;

            using is_sender = void;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, R> { std::forward<Rec>(rec), std::move(sender.mError) };
            }

            R mError;
        };

        template <typename R>
        auto operator()(R && error) const
        {
            return sender<R> { std::forward<R>(error) };
        }
    };

    template <typename... T>
    inline constexpr just_error_t<T...> just_error;

    struct then_t {

        template <typename Rec, typename T>
        struct receiver : algorithm_receiver<Rec> {

            template <typename... V>
            void set_value(V &&...values)
            {
                using V2 = decltype(TupleUnpacker::invoke(mTransform, std::forward<V>(values)...));
                if constexpr (std::same_as<V2, void>) {
                    TupleUnpacker::invoke(mTransform, std::forward<V>(values)...);
                    this->mRec.set_value();
                } else if constexpr (InstanceOf<V2, std::tuple>)
                    TupleUnpacker::invokeExpand(LIFT(this->mRec.set_value, this), TupleUnpacker::invoke(mTransform, std::forward<V>(values)...));
                else
                    this->mRec.set_value(TupleUnpacker::invoke(mTransform, std::forward<V>(values)...));
            }

            T mTransform;
        };

        template <typename T>
        static auto return_types_helper() {
            if constexpr (std::same_as<T, void>) {
                return type_pack<> {};
            } else if constexpr (InstanceOf<T, std::tuple>) {
                return to_type_pack<T> {};
            } else {
                return type_pack<T> {};
            }
        }

        template <typename Sender, typename T>
        struct sender : algorithm_sender<Sender> {

            template <typename... V>
            using helper = decltype(TupleUnpacker::invoke(std::declval<T>(), std::declval<V>()...));
            using helper_type = typename Sender::template value_types<helper>;
            template <template <typename...> typename Tuple>
            using value_types = typename decltype(return_types_helper<helper_type>())::template instantiate<Tuple>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return algorithm_state<Sender, receiver<Rec, T>> { std::forward<Sender>(sender.mSender), std::forward<Rec>(rec), std::forward<T>(sender.mTransform) };
            }

            Sender mSender;
            T mTransform;
        };

        template <typename Sender, typename T>
        friend auto tag_invoke(then_t, Sender &&inner, T &&transform)
        {
            return sender<Sender, T> { {}, std::forward<Sender>(inner), std::forward<T>(transform) };
        }

        template <typename Sender, typename T>
        requires tag_invocable<then_t, Sender, T>
        auto operator()(Sender &&sender, T &&transform) const
            noexcept(is_nothrow_tag_invocable_v<then_t, Sender, T>)
                -> tag_invoke_result_t<then_t, Sender, T>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<T>(transform));
        }

        template <typename T>
        auto operator()(T &&transform) const
        {
            return pipable_from_right(*this, std::forward<T>(transform));
        }

        template <typename T>
        struct typed {
            template <typename Sender>
            requires tag_invocable<then_t, Sender, T>
            auto operator()(Sender &&sender, T &&transform = {}) const
                noexcept(is_nothrow_tag_invocable_v<then_t, Sender, T>)
                    -> tag_invoke_result_t<then_t, Sender, T>
            {
                return tag_invoke(then_t {}, std::forward<Sender>(sender), std::forward<T>(transform));
            }

            auto operator()(T &&transform = {}) const
            {
                return pipable_from_right(then_t {}, std::forward<T>(transform));
            }
        };
    };

    inline constexpr then_t then;

    template <typename T>
    inline constexpr then_t::typed<T> typed_then;

    struct finally_t {

        template <typename Rec, typename F>
        struct receiver : algorithm_receiver<Rec> {

            template <typename... V>
            void set_value(V &&...values)
            {
                mFinally();
                this->mRec.set_value(std::forward<V>(values)...);
            }

            template <typename... R>
            void set_error(R &&...errors) {
                mFinally();
                this->mRec.set_error(std::forward<R>(errors)...);
            }

            void set_done() {
                mFinally();
                this->mRec.set_done();
            }

            F mFinally;
        };

        template <typename Sender, typename F>
        struct sender : algorithm_sender<Sender> {

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return algorithm_state<Sender, receiver<Rec, F>> { std::forward<Sender>(sender.mSender), std::forward<Rec>(rec), std::forward<F>(sender.mFinally) };
            }

            Sender mSender;
            F mFinally;
        };

        template <typename Sender, typename F>
        friend auto tag_invoke(finally_t, Sender &&inner, F &&finally)
        {
            return sender<Sender, F> { {}, std::forward<Sender>(inner), std::forward<F>(finally) };
        }

        template <typename Sender, typename F>
        requires tag_invocable<finally_t, Sender, F>
        auto operator()(Sender &&sender, F &&finally) const
            noexcept(is_nothrow_tag_invocable_v<finally_t, Sender, F>)
                -> tag_invoke_result_t<finally_t, Sender, F>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<F>(finally));
        }

        template <typename F>
        auto operator()(F &&finally) const
        {
            return pipable_from_right(*this, std::forward<F>(finally));
        }

        template <typename F>
        struct typed {
            template <typename Sender>
            requires tag_invocable<finally_t, Sender, F>
            auto operator()(Sender &&sender, F &&finally = {}) const
                noexcept(is_nothrow_tag_invocable_v<finally_t, Sender, F>)
                    -> tag_invoke_result_t<finally_t, Sender, F>
            {
                return tag_invoke(finally_t {}, std::forward<Sender>(sender), std::forward<F>(finally));
            }

            auto operator()(F &&finally = {}) const
            {
                return pipable_from_right(finally_t {}, std::forward<F>(finally));
            }
        };
    };

    inline constexpr finally_t finally;

    template <typename F>
    inline constexpr finally_t::typed<F> typed_finally;

    struct then_receiver_t {

        template <typename Sender, typename Rec1, typename Rec2>
        struct state;

        template <typename Sender, typename Rec1, typename Rec2>
        struct receiver : algorithm_receiver<Rec2> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(this->mRec, std::forward<R>(result)...);
            }

            state<Sender, Rec1, Rec2> *mState;
        };

        template <typename Sender, typename Rec1, typename Rec2>
        struct state : algorithm_state<Sender, receiver<Sender, Rec1, Rec2>> {
            using base = algorithm_state<Sender, receiver<Sender, Rec1, Rec2>>;

            state(Sender &&sender, Rec1 &&rec1, Rec2 &&rec2)
                : base(std::forward<Sender>(sender), std::forward<Rec2>(rec2), this)
                , mRec1(std::forward<Rec1>(rec1))
            {
            }

            template <typename... V>
            void set_value(Rec2 &rec, V &&...values)
            {
                mRec1.set_value(values...);
                rec.set_value(std::forward<V>(values)...);
            }

            void set_done(Rec2 &rec)
            {
                mRec1.set_done();
                rec.set_done();
            }

            template <typename... R>
            void set_error(Rec2 &rec, R &&...result)
            {
                mRec1.set_error(result...);
                rec.set_error(std::forward<R>(result)...);
            }

            Rec1 mRec1;
        };

        template <typename Sender, typename Rec1>
        struct sender : algorithm_sender<Sender> {

            template <typename Rec2>
            friend auto tag_invoke(connect_t, sender &&sender, Rec2 &&rec)
            {
                return state<Sender, Rec1, Rec2> { std::forward<Sender>(sender.mSender), std::forward<Rec1>(sender.mRec1), std::forward<Rec2>(rec) };
            }

            Sender mSender;
            Rec1 mRec1;
        };

        template <typename Sender, typename Rec1>
        friend auto tag_invoke(then_receiver_t, Sender &&inner, Rec1 &&rec1)
        {
            return sender<Sender, Rec1> { {}, std::forward<Sender>(inner), std::forward<Rec1>(rec1) };
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
        struct state;

        template <typename Rec, typename C, typename F>
        struct receiver : algorithm_receiver<Rec &> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(this->mRec, std::forward<R>(result)...);
            }

            state<Rec, C, F> *mState;
        };

        template <typename Rec, typename C, typename F>
        struct state : base_state<Rec> {
            using value_type = typename std::remove_reference_t<C>::value_type &;
            using inner_sender_t = std::invoke_result_t<F, value_type>;
            using inner_state_t = connect_result_t<inner_sender_t, receiver<Rec, C, F>>;

            state(Rec &&rec, C &&c, F &&f)
                : base_state<Rec>(std::forward<Rec>(rec))
                , mF(std::forward<F>(f))
                , mC(std::forward<C>(c))
            {
            }

            ~state() { }

            void start()
            {
                mIt = mC.begin();
                loop(this->mRec);
            }

            void stopInner()
            {
                mState.~inner_state_t();
            }

            void set_value(Rec &rec)
            {
                stopInner();
                if (mFlag.test_and_set())
                    loop(rec);
            }

            void set_done(Rec &rec)
            {
                stopInner();
                rec.set_done();
            }

            template <typename... R>
            void set_error(Rec &rec, R... result)
            {
                stopInner();
                rec.set_error(result...);
            }

            void loop(Rec &rec)
            {
                while (proceed(rec))
                    ++mIt;
            }

            bool proceed(Rec &rec)
            {
                if (mIt == mC.end()) {
                    rec.set_value();
                    return false;
                } else {
                    mFlag.clear();
                    new (&mState) inner_state_t { connect(mF(*mIt), receiver<Rec, C, F> { rec, this }) };
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
        struct sender {
            using value_type = typename std::remove_reference_t<C>::value_type &;
            using inner_sender_t = std::invoke_result_t<F, value_type>;
            using result_type = typename inner_sender_t::result_type;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, C, F> { std::forward<Rec>(rec), std::forward<C>(sender.mContainer), std::forward<F>(sender.mF) };
            }

            C mContainer;
            F mF;
        };

        template <typename C, typename F>
        friend auto tag_invoke(for_each_t, C &&c, F &&f)
        {
            return sender<C, F> { std::forward<C>(c), std::forward<F>(f) };
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

        template <typename Rec, typename Sender>
        struct state;

        template <typename Rec, typename Sender>
        struct receiver : algorithm_receiver<Rec> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(this->mRec, std::forward<R>(result)...);
            }

            state<Rec, Sender> *mState;
        };

        template <typename Rec, typename Sender>
        struct state : algorithm_state<Sender, receiver<Rec, Sender>> {
            using base = algorithm_state<Sender, receiver<Rec, Sender>>;

            state(Rec &&rec, Sender &&sender)
                : base(std::forward<Sender>(sender), std::forward<Rec>(rec), this)
            {
            }

            ~state() { }

            template <typename... V>
            void set_value(Rec &, V &&...args)
            {
                this->start();
            }
        };

        template <typename Sender>
        struct sender : algorithm_sender<Sender> {

            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Sender> { std::forward<Rec>(rec), std::forward<Sender>(sender.mSender) };
            }

            Sender mSender;
        };

        template <typename Sender>
        friend auto tag_invoke(repeat_t, Sender &&inner)
        {
            return sender<Sender> { {}, std::forward<Sender>(inner) };
        }

        template <typename Sender>
        requires tag_invocable<repeat_t, Sender>
        auto operator()(Sender &&sender) const
            noexcept(is_nothrow_tag_invocable_v<repeat_t, Sender>)
                -> tag_invoke_result_t<repeat_t, Sender>
        {
            return tag_invoke(*this, std::forward<Sender>(sender));
        }

        template <typename Sender>
        friend auto operator|(Sender &&sender, repeat_t r)
        {
            return tag_invoke(r, std::forward<Sender>(sender));
        }
    };

    inline constexpr repeat_t repeat;

    struct when_all_t {

        template <typename Rec, typename Is, typename... Sender>
        struct state;

        template <size_t I, typename Rec, typename Is, typename... Sender>
        struct receiver : algorithm_receiver<Rec &> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->template set_value<I>(this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(this->mRec, std::forward<R>(result)...);
            }

            state<Rec, Is, Sender...> *mState;
        };

        template <size_t I, typename Sender, typename Rec, typename Is, typename... Senders>
        struct inner_state {

            using State = connect_result_t<Sender, receiver<I, Rec, Is, Senders...>>;

            inner_state(auto &&ctor)
                : mState { ctor() }
            {
            }

            ~inner_state() { }

            void start()
            {
                mState.start();
            }

            State mState;
        };

        template <typename Rec, size_t... Is, typename... Sender>
        struct state<Rec, std::index_sequence<Is...>, Sender...> : base_state<Rec> {

            using Tuple = std::tuple<typename Sender::template value_types<identity>...>;
            using R = typename first_t<Sender...>::result_type;

            state(Rec &&rec, Sender &&...senders)
                : base_state<Rec> { std::forward<Rec>(rec) }
                , mStates { [&]() { return connect(std::forward<Sender>(senders), receiver<Is, Rec, std::index_sequence<Is...>, Sender...> { this->mRec, this }); }... }
            {
            }

            ~state() { }

            void start()
            {
                mCompleteCount = 0;
                mState = OK;
                TupleUnpacker::forEach(mStates, [](auto &state) { state.start(); });
                mark_complete(this->mRec);
            }

            template <size_t I, typename V>
            void set_value(Rec &rec, V &&value)
            {
                if (mState == OK)
                    std::get<I>(mValues) = std::forward<V>(value);
                mark_complete(rec);
            }

            void set_done(Rec &rec)
            {
                mState = DONE;
                mark_complete(rec);
            }

            void set_error(Rec &rec, patch_void_t<R> result)
            {
                State expected = OK;
                if (mState.compare_exchange_strong(expected, ERROR)) {
                    mResult = result;
                }
                mark_complete(rec);
            }

            void mark_complete(Rec &rec)
            {
                size_t count = mCompleteCount.fetch_add(1);
                if (count == sizeof...(Is)) {
                    switch (mState) {
                    case OK:
                        TupleUnpacker::invokeFromTuple(LIFT(rec.set_value, &), Tuple { std::move(mValues) });
                        break;
                    case ERROR:
                        rec.set_error(mResult);
                        break;
                    case DONE:
                        rec.set_done();
                        break;
                    default:
                        throw 0;
                    }
                }
            }

            std::tuple<inner_state<Is, Sender, Rec, std::index_sequence<Is...>, Sender...>...> mStates;
            patch_void_t<R> mResult;
            template <typename V>
            using helper = std::conditional_t<std::is_reference_v<V>, OutRef<std::remove_reference_t<V>>, V>;
            std::tuple<typename Sender::template value_types<helper>...> mValues;
            enum State { OK,
                ERROR,
                DONE };
            std::atomic<State> mState = OK;
            std::atomic<size_t> mCompleteCount = 0;
        };

        template <typename... Sender>
        struct sender {

            using result_type = typename first_t<Sender...>::result_type;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<typename Sender::template value_types<identity>...>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return TupleUnpacker::constructExpand<state<Rec, std::index_sequence_for<Sender...>, Sender...>>(std::forward<Rec>(rec), std::move(sender.mSenders));
            };

            std::tuple<Sender...> mSenders;
        };

        template <typename... Sender>
        friend auto tag_invoke(when_all_t, Sender &&...senders)
        {
            return sender<Sender...> { { std::move(senders)... } };
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

        template <size_t Is>
        struct inner_tag {
        };

        template <typename Rec, typename... Sender>
        struct state;

        template <typename Rec, typename Tag, typename... Sender>
        struct receiver : algorithm_receiver<Rec &> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(Tag {}, this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(Tag {}, this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(Tag {}, this->mRec, std::forward<R>(result)...);
            }

            state<Rec, Sender...> *mState;
        };

        template <typename Rec, typename... Sender>
        struct state : base_state<Rec> {

            template <size_t I>
            using inner_rec = receiver<Rec, inner_tag<I>, Sender...>;

            template <size_t... Is>
            static auto stateTupleHelper(std::index_sequence<Is...>) -> std::tuple<connect_result_t<Sender, inner_rec<Is>>...> { }
            using StateTuple = decltype(stateTupleHelper(std::make_index_sequence<sizeof...(Sender)> {}));

            state(Rec &&rec, std::tuple<Sender...> &&senders)
                : base_state<Rec> { std::forward<Rec>(rec) }
                , mSenders(std::move(senders))
            {
            }

            ~state() { }

            void start()
            {
                if constexpr (sizeof...(Sender) == 0)
                    base_state<Rec>::set_value();
                else {
                    new (&std::get<0>(mStates)) std::tuple_element_t<0, StateTuple>(Execution::connect(std::move(std::get<0>(mSenders)), receiver<Rec, inner_tag<0>, Sender...> { this->mRec, this }));
                    std::get<0>(mStates).start();
                }
            }

            template <size_t I, typename... V>
            void set_value(inner_tag<I>, Rec &rec, V &&...values)
            {
                using E = std::tuple_element_t<I, StateTuple>;
                std::get<I>(mStates).~E();
                if constexpr (sizeof...(Sender) == I + 1)
                    rec.set_value();
                else {
                    new (&std::get<I + 1>(mStates)) std::tuple_element_t<I + 1, StateTuple>(Execution::connect(std::move(std::get<I + 1>(mSenders)), receiver<Rec, inner_tag<I + 1>, Sender...> { this->mRec, this }));
                    std::get<I + 1>(mStates).start();
                }
            }

            template <size_t I>
            void set_done(inner_tag<I>, Rec &rec)
            {
                using E = std::tuple_element_t<I, StateTuple>;
                std::get<I>(mStates).~E();
                rec.set_done();
            }

            template <size_t I, typename... R>
            void set_error(inner_tag<I>, Rec &rec, R &&...result)
            {
                using E = std::tuple_element_t<I, StateTuple>;
                std::get<I>(mStates).~E();
                rec.set_error(std::forward<R>(result)...);
            }

            std::tuple<Sender...> mSenders;

            union {
                StateTuple mStates;
            };
        };

        template <typename... Sender>
        struct sender : base_sender {
            using result_type = void;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Sender...> { std::forward<Rec>(rec), std::move(sender.mSenders) };
            }

            std::tuple<Sender...> mSenders;
        };

        template <typename Sender>
        friend decltype(auto) tag_invoke(sequence_t, Sender &&sender)
        {
            return std::forward<Sender>(sender);
        }

        template <typename... Sender>
        friend auto tag_invoke(sequence_t sequence, Sender &&...senders)
        {
            return sender<Sender...> { {}, { std::forward<Sender>(senders)... } };
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

    /* template <Sender S1, Sender S2>
    requires tag_invocable<sequence_t, S1, S2>
    auto operator|(S1 &&sender1, S2 &&sender2)
    {
        return sequence(std::forward<S1>(sender1), std::forward<S2>(sender2));
    }*/

    struct let_value_t {

        template <typename Rec, typename Sender, typename F>
        struct state;

        template <typename Rec, typename Sender, typename F, typename QualifiedRec, typename... Tags>
        struct receiver : algorithm_receiver<QualifiedRec> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(Tags {}..., this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(Tags {}..., this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(Tags {}..., this->mRec, std::forward<R>(result)...);
            }

            state<Rec, Sender, F> *mState;
        };

        template <typename Rec, typename Sender, typename F>
        struct state : algorithm_state<Sender, receiver<Rec, Sender, F, Rec>> {
            using base = algorithm_state<Sender, receiver<Rec, Sender, F, Rec>>;

            struct inner_tag {
            };
            template <typename... V>
            using helper = std::invoke_result_t<F, V...>;
            using inner_sender_t = typename Sender::template value_types<helper>;
            using Tuple = typename Sender::template value_types<std::tuple>;
            using inner_state = connect_result_t<inner_sender_t, receiver<Rec, Sender, F, Rec &, inner_tag>>;

            state(Rec &&rec, Sender &&sender, F &&f)
                : algorithm_state<Sender, receiver<Rec, Sender, F, Rec>> { std::forward<Sender>(sender), std::forward<Rec>(rec), this }
                , mF(std::forward<F>(f))
            {
            }

            ~state() { }

            template <typename... V>
            void set_value(Rec &rec, V &&...values)
            {
                new (&mValue) Tuple { { std::forward<V>(values)... } };
                new (&mInnerState) inner_state { connect(TupleUnpacker::invokeFromTuple(mF, mValue), receiver<Rec, Sender, F, Rec &, inner_tag> { rec, this }) };
                mInnerState.start();
            }
            using base::set_done;
            using base::set_error;

            void cleanup()
            {
                mInnerState.~inner_state();
                mValue.~tuple();
            }

            template <typename... V2>
            void set_value(inner_tag, Rec &rec, V2 &&...values)
            {
                cleanup();
                rec.set_value(std::forward<V2>(values)...);
            }

            void set_done(inner_tag, Rec &rec)
            {
                cleanup();
                rec.set_done();
            }

            template <typename... R>
            void set_error(inner_tag, Rec &rec, R &&...result)
            {
                cleanup();
                rec.set_error(std::forward<R>(result)...);
            }

            F mF;
            union {
                Tuple mValue;
            };
            union {
                inner_state mInnerState;
            };
        };

        template <typename Sender, typename F>
        struct sender : algorithm_sender<Sender> {
            template <typename... V>
            using helper = std::invoke_result_t<F, V...>;
            using inner_sender_t = typename Sender::template value_types<helper>;

            template <template <typename...> typename Tuple>
            using value_types = typename inner_sender_t::template value_types<Tuple>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Sender, F> { std::forward<Rec>(rec), std::forward<Sender>(sender.mSender), std::forward<F>(sender.mF) };
            }

            Sender mSender;
            F mF;
        };

        template <typename Sender, typename F>
        friend auto tag_invoke(let_value_t, Sender &&inner, F &&f)
        {
            return sender<Sender, F> { {}, std::forward<Sender>(inner), std::forward<F>(f) };
        }

        template <typename Sender, typename F>
        requires tag_invocable<let_value_t, Sender, F>
        auto operator()(Sender &&sender, F &&f) const
            noexcept(is_nothrow_tag_invocable_v<let_value_t, Sender, F>)
                -> tag_invoke_result_t<let_value_t, Sender, F>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<F>(f));
        }

        template <typename F>
        auto operator()(F &&f) const
        {
            return pipable_from_right(*this, std::forward<F>(f));
        }
    };

    inline constexpr let_value_t let_value;

    struct assign_t {

        template <typename Rec, typename Sender, typename T>
        struct state;

        template <typename Rec, typename Sender, typename T>
        struct receiver : algorithm_receiver<Rec> {

            template <typename... V>
            void set_value(V &&...value)
            {
                mState->set_value(this->mRec, std::forward<V>(value)...);
            }

            void set_done()
            {
                mState->set_done(this->mRec);
            }

            template <typename... R>
            void set_error(R &&...result)
            {
                mState->set_error(this->mRec, std::forward<R>(result)...);
            }

            state<Rec, Sender, T> *mState;
        };

        template <typename Rec, typename Sender, typename T>
        struct state : algorithm_state<Sender, receiver<Rec, Sender, T>> {

            state(Rec &&rec, Sender &&sender, T &&var)
                : algorithm_state<Sender, receiver<Rec, Sender, T>>(std::forward<Sender>(sender), std::forward<Rec>(rec), this)
                , mVar(std::forward<T>(var))
            {
            }

            template <typename V>
            void set_value(Rec &rec, V &&value)
            {
                mVar = std::forward<V>(value);
                rec.set_value();
            }

            T mVar;
        };

        template <typename Sender, typename T>
        struct sender : algorithm_sender<Sender> {

            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Sender, T> { std::forward<Rec>(rec), std::forward<Sender>(sender.mSender), std::forward<T>(sender.mVar) };
            }

            Sender mSender;
            T mVar;
        };

        template <typename Sender, typename T>
        friend auto tag_invoke(assign_t, Sender &&inner, T &&var)
        {
            return sender<Sender, T> { {}, std::forward<Sender>(inner), std::forward<T>(var) };
        }

        template <typename Sender, typename T>
        auto operator()(Sender &&sender, T &&var) const
            noexcept(is_nothrow_tag_invocable_v<assign_t, Sender, T>)
                -> tag_invoke_result_t<assign_t, Sender, T>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<T>(var));
        }

        template <typename T>
        auto operator()(T &var) const
        {
            return pipable_from_right(*this, var);
        }
    };

    inline constexpr assign_t assign;

    template <typename R>
    struct to_result_t {

        template <typename Rec, typename Sender>
        struct state;

        template <typename Rec, typename Sender>
        struct receiver : algorithm_receiver<Rec> {

            template <typename... V>
            void set_value(V &&...value)
            {
                this->mRec.set_value(mOnValue, std::forward<V>(value)...);
            }

            void set_done()
            {
                this->mRec.set_value(mOnDone);
            }

            template <typename... R2>
            void set_error(R fixedResult, R2 &&...result)
            {
                this->mRec.set_error(fixedResult, std::forward<R>(result)...);
            }

            R mOnValue;
            R mOnDone;
        };

        template <typename Rec, typename Sender>
        struct state : algorithm_state<Sender, receiver<Rec, Sender>> {

            state(Rec &&rec, Sender &&sender, R onValue, R onDone)
                : algorithm_state<Sender, receiver<Rec, Sender>>(std::forward<Sender>(sender), std::forward<Rec>(rec), onValue, onDone)
            {
            }
        };

        template <typename Sender>
        struct sender : algorithm_sender<Sender> {

            template <template <typename...> typename Tuple>
            using value_types = typename Sender::template value_types<type_pack>::template prepend<R>::template instantiate<Tuple>;

            template <typename Rec>
            friend auto tag_invoke(connect_t, sender &&sender, Rec &&rec)
            {
                return state<Rec, Sender> { std::forward<Rec>(rec), std::forward<Sender>(sender.mSender), sender.mOnValue, sender.mOnDone };
            }

            Sender mSender;
            R mOnValue;
            R mOnDone;
        };

        template <typename Sender>
        friend auto tag_invoke(to_result_t, Sender &&inner, R onValue, R onDone)
        {
            return sender<Sender> { {}, std::forward<Sender>(inner), onValue, onDone };
        }

        template <typename Sender>
        auto operator()(Sender &&sender, R onValue, R onDone) const
            noexcept(is_nothrow_tag_invocable_v<to_result_t, Sender, R, R>)
                -> tag_invoke_result_t<to_result_t, Sender, R, R>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), onValue, onDone);
        }

        auto operator()(R onValue, R onDone) const
        {
            return pipable_from_right(*this, onValue, onDone);
        }
    };

    template <typename R>
    inline constexpr to_result_t<R> to_result;
}
}