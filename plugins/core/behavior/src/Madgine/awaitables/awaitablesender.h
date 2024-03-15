#pragma once

#include "Generic/execution/algorithm.h"
#include "Generic/execution/sender.h"
#include "Generic/execution/storage.h"
#include "Generic/makeowning.h"
#include "Generic/withresult.h"

#include "Madgine/debug/debuggablesender.h"



namespace Engine {

template <typename Sender>
struct BehaviorAwaitableSender;

template <typename Sender>
struct BehaviorAwaitableReceiver : Execution::execution_receiver<> {

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

    template <typename O>
    friend bool tag_invoke(Execution::resolve_var_d_t, BehaviorAwaitableReceiver &rec, std::string_view name, O &out)
    {
        ValueType v;
        if (rec.mBehavior->mReceiver->resolveVar(name, v)) {
            out = v.as<O>();
            return true;
        } else {
            return false;
        }
    }

    friend std::stop_token tag_invoke(Execution::get_stop_token_t, BehaviorAwaitableReceiver &rec)
    {
        return rec.mBehavior->mReceiver->stopToken();
    }

    friend Debug::ParentLocation *tag_invoke(Execution::get_debug_location_t, BehaviorAwaitableReceiver &rec)
    {
        return &rec.mBehavior->mDebugLocation;
    }

    BehaviorAwaitableSender<Sender> *mState;
    CoroutineBehaviorState *mBehavior;
};

template <typename Sender>
struct BehaviorAwaitableSender {

    auto buildState(Sender &&sender, CoroutineBehaviorState *state)
    {
        return Execution::connect(std::forward<Sender>(sender) | Execution::with_debug_location<Execution::SenderLocation>(), BehaviorAwaitableReceiver<Sender> { {}, this, state });
    }

    using S = std::invoke_result_t<decltype(&BehaviorAwaitableSender::buildState), BehaviorAwaitableSender, Sender, nullptr_t>;

    BehaviorAwaitableSender(Sender &&sender, CoroutineBehaviorState *state)
        : mState(buildState(std::forward<Sender>(sender), state))
    {
    }

    bool await_ready()
    {
        mState.start();
        return mFlag.test() && mResult.is_value();
    }

    bool await_suspend(std::coroutine_handle<CoroutineBehaviorState> behavior)
    {
        mBehavior = behavior;
        if (mFlag.test_and_set()) {
            if (mResult.is_value()) {
                return false;
            } else if (mResult.is_error()){
                mResult.reproduce_error(mBehavior.promise());
            } else {
                mBehavior.promise().set_done();
            }
        } else {
            return true;
        }
    }

    Execution::ValueStorage<Sender> await_resume()
    {
        return std::move(mResult).value();
    }

    template <typename... V>
    void set_value(V &&... v)
    {
        mResult.set_value(std::forward<V>(v)...);
        if (mFlag.test_and_set())
            mBehavior.resume();
    }

    void set_done()
    {
        mResult.set_done();
        if (mFlag.test_and_set())
            mBehavior.promise().set_done();            
    }

    template <typename... R>
    void set_error(R &&... error)
    {
        mResult.set_error(std::forward<R>(error)...);
        if (mFlag.test_and_set())
            mResult.reproduce_error(mBehavior.promise());
    }

private:
    S mState;
    std::atomic_flag mFlag = ATOMIC_FLAG_INIT;
    std::coroutine_handle<CoroutineBehaviorState> mBehavior;
    Execution::ResultStorage<Sender> mResult;
};

}
