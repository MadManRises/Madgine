#pragma once

#include "Generic/genericresult.h"

#include "Generic/execution/virtualsender.h"

#include "Generic/execution/sender.h"

#include "Meta/keyvalue/keyvaluereceiver.h"

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Meta/keyvalue/virtualscope.h"

#include "Generic/execution/algorithm.h"

#include "Generic/execution/state.h"

#include "Meta/keyvalue/valuetype.h"

#include "behaviortracker.h"

#include "Interfaces/debug/stacktrace.h"

#include "Generic/delayedconstruct.h"

namespace Engine {

ENUM_BASE(InterpretResult, GenericResult);

template <typename T>
concept UntypedBehavior = Execution::Sender<T> || true /*is Algorithm*/;

template <typename T, typename R>
concept TypedBehavior = UntypedBehavior<T>;

template <typename Algorithm>
struct AlgorithmBehaviorState;

struct CoroutineBehaviorState;

struct MADGINE_BEHAVIOR_EXPORT Behavior {

    static void destroyState(BehaviorStateBase *state);
    using StatePtr = std::unique_ptr<BehaviorStateBase, Functor<&destroyState>>;

    Behavior() = default;
    Behavior(StatePtr state);

    template <typename Algorithm>
    Behavior(Algorithm &&alg)
        : mState(new AlgorithmBehaviorState<Algorithm>(std::forward<Algorithm>(alg)))
    {
    }

    Behavior &operator=(StatePtr state);

    StatePtr release()
    {
        return std::move(mState);
    }

    struct receiver {
        void set_value(ArgumentList arguments)
        {
            set_value_inner(std::move(arguments));
        }
        virtual void set_value_inner(ArgumentList arguments) = 0;
        virtual void set_error(InterpretResult r) = 0;
        virtual void set_done() = 0;

        virtual bool resolveVar(ValueType &retVal, std::string_view name) = 0;
    };

    template <typename Rec>
    struct receiverImpl : Execution::algorithm_receiver<Rec>, receiver {

        receiverImpl(Rec &&rec, StatePtr state)
            : Execution::algorithm_receiver<Rec> { std::forward<Rec>(rec) }
            , mState(std::move(state))
        {
        }

        void set_value(ArgumentList arguments)
        {
            mState->start(this, std::move(arguments), get_behavior_context(this->mRec), Execution::get_stop_token(this->mRec));
        }

        void set_value_inner(ArgumentList arguments) override
        {
            this->mRec.set_value(std::move(arguments));
        }
        void set_error(InterpretResult r) override
        {
            this->mRec.set_error(r);
        }
        void set_done() override
        {
            this->mRec.set_done();
        }

        bool resolveVar(ValueType &retVal, std::string_view name) override
        {
            return Execution::resolve_var_d(this->mRec, name, retVal);
        }

        StatePtr mState;
    };

    template <Execution::Sender Inner, typename Rec>
    struct state : Execution::algorithm_state<Inner, receiverImpl<Rec>> {
        state(Inner &&inner, Rec &&rec, StatePtr state)
            : Execution::algorithm_state<Inner, receiverImpl<Rec>> { std::forward<Inner>(inner), std::forward<Rec>(rec), std::move(state) }
        {
        }

        template <typename F>
        friend void tag_invoke(Execution::visit_state_t, state &state, F &&f)
        {
            Execution::get_receiver(state).mState->visit(std::forward<F>(f));
        }
                        
        friend std::string tag_invoke(get_behavior_name_t, const state &state)
        {
            return Execution::get_receiver(state).mState->name();
        }
    };

    template <Execution::Sender Inner>
    struct sender : Execution::algorithm_sender<Inner> {

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
        {
            return state<Inner, Rec> { std::forward<Inner>(sender.mInner), std::forward<Rec>(rec), std::move(sender.mState) };
        }

        Inner mInner;
        StatePtr mState;
    };

    template <Execution::Sender Inner>
    auto operator()(Inner &&inner) &&
    {
        assert(mState);
        return sender<Inner> { {}, std::forward<Inner>(inner), std::move(mState) };
    }

    template <Execution::Sender Inner>
    friend auto operator|(Inner &&inner, Behavior &&behavior)
    {
        assert(behavior.mState);
        return sender<Inner> { {}, std::forward<Inner>(inner), std::move(behavior.mState) };
    }

    using promise_type = CoroutineBehaviorState;

    StatePtr mState;
};

struct BehaviorStateBase {
    virtual ~BehaviorStateBase() = default;

    virtual void start(Behavior::receiver *rec, ArgumentList arguments, BehaviorTrackerContext context, std::stop_token stopToken) = 0;
    virtual void destroy()
    {
        delete this;
    }
    virtual void visit(CallableView<void(const Execution::StateDescriptor &)> visitor) = 0;
    virtual std::string name() const = 0;
};

template <typename Sender, typename... V>
struct BehaviorAwaitableSenderImpl;

template <typename Sender>
using BehaviorAwaitableSender = typename Sender::template value_types<type_pack>::template prepend<Sender>::template instantiate<BehaviorAwaitableSenderImpl>;

struct MADGINE_BEHAVIOR_EXPORT CoroutineBehaviorState : BehaviorStateBase {

    Behavior get_return_object();

    void start(Behavior::receiver *rec, ArgumentList arguments, BehaviorTrackerContext context, std::stop_token stopToken) override;
    void destroy() override;
    void visit(CallableView<void(const Execution::StateDescriptor &)> visitor) override;

    struct MADGINE_BEHAVIOR_EXPORT InitialSuspend {
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<CoroutineBehaviorState> handle) noexcept;
        void await_resume() noexcept;
    };

    struct MADGINE_BEHAVIOR_EXPORT FinalSuspend {
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<CoroutineBehaviorState> handle) noexcept;
        void await_resume() noexcept;
    };

    InitialSuspend initial_suspend() noexcept;
    FinalSuspend final_suspend() noexcept;

    void return_void();
    void unhandled_exception();
    void set_error(InterpretResult result);
    void set_done();

    template <typename T>
    decltype(auto) await_transform(T &&awaitable)
    {
        if constexpr (Execution::Sender<std::remove_reference_t<T>>) {
            return BehaviorAwaitableSender<T> { std::forward<T>(awaitable), this };
        } else {
            return std::forward<T>(awaitable);
        }
    }

    virtual std::string name() const override;

    Debug::StackTrace<1> mStacktrace;

    Behavior::receiver *mReceiver = nullptr;
    ArgumentList mArguments;
    BehaviorTrackerContext mContext;
    std::stop_token mStopToken;
};

template <typename Algorithm>
struct AlgorithmBehaviorState : BehaviorStateBase {

    struct receiver {

        void set_value(ArgumentList args)
        {
            mReceiver->set_value(std::move(args));
        }
        void set_error(InterpretResult r)
        {
            mReceiver->set_error(r);
        }
        void set_done()
        {
            mReceiver->set_done();
        }

        template <typename O>
        friend auto tag_invoke(Execution::resolve_var_d_t, receiver &rec, std::string_view name, O &out)
        {
            ValueType v;
            if (rec.mReceiver->resolveVar(v, name)) {
                out = v.as<O>();
                return true;
            } else {
                return false;
            }
        }

        friend std::stop_token tag_invoke(Execution::get_stop_token_t, receiver &rec)
        {
            return rec.mStopToken;
        }

        Behavior::receiver *mReceiver;
        std::stop_token mStopToken;
    };

    using State = Execution::connect_result_t<decltype(Execution::just(std::declval<ArgumentList &>()) | std::declval<Algorithm>()), receiver>;

    AlgorithmBehaviorState(Algorithm &&alg)
        : mData(std::forward<Algorithm>(alg))
    {
    }

    void start(Behavior::receiver *rec, ArgumentList arguments, BehaviorTrackerContext context, std::stop_token stopToken) override
    {
        Algorithm algorithm = std::get<Algorithm>(mData);
        mData = DelayedConstruct<State> { [&]() { return Execution::connect(Execution::just(std::move(arguments)) | std::forward<Algorithm>(algorithm), receiver { rec, std::move(stopToken) }); } };
        std::get<State>(mData).start();
    }

    void visit(CallableView<void(const Execution::StateDescriptor &)> visitor) override
    {
        Execution::visit_state(std::get<State>(mData), visitor);
    }

    std::string name() const override
    {
        return get_behavior_name(std::get<State>(mData));
    }

    std::variant<Algorithm, State> mData;
};
}

REGISTER_TYPE(Engine::BehaviorStateBase);