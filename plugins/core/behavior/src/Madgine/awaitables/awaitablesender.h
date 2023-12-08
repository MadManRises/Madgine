#pragma once

#include "Generic/execution/algorithm.h"
#include "Generic/execution/sender.h"
#include "Generic/makeowning.h"
#include "Generic/withresult.h"

namespace Engine {

template <typename Sender, typename... V>
struct BehaviorAwaitableSenderImpl;

template <typename Sender>
using BehaviorAwaitableSender = typename Sender::template value_types<type_pack>::template prepend<Sender>::template instantiate<BehaviorAwaitableSenderImpl>;

template <typename Sender, typename... V>
struct BehaviorAwaitableReceiver : Execution::execution_receiver<> {

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

    friend BehaviorTrackerContext tag_invoke(get_behavior_context_t, BehaviorAwaitableReceiver &rec)
    {
        return rec.mBehavior->mContext;
    }

    template <typename O>
    friend bool tag_invoke(Execution::resolve_var_d_t, BehaviorAwaitableReceiver &rec, std::string_view name, O &out)
    {
        ValueType v;
        if (rec.mBehavior->mReceiver->resolveVar(v, name)) {
            out = v.as<O>();
            return true;
        } else {
            return false;
        }
    }

    friend std::stop_token tag_invoke(Execution::get_stop_token_t, BehaviorAwaitableReceiver &rec)
    {
        return rec.mBehavior->mStopToken;
    }

    BehaviorAwaitableSender<Sender> *mState;
    CoroutineBehaviorState *mBehavior;
};

template <typename Sender, typename... V>
struct BehaviorAwaitableSenderImpl;

template <typename Sender>
struct BehaviorAwaitableSenderImpl<Sender> {

    using S = Execution::connect_result_t<BehaviorTracker::sender<Sender>, BehaviorAwaitableReceiver<Sender>>;
    using R = typename Sender::result_type;

    BehaviorAwaitableSenderImpl(Sender &&sender, CoroutineBehaviorState *state)
        : mState(Execution::connect(std::forward<Sender>(sender) | state->mContext, BehaviorAwaitableReceiver<Sender> { {}, this, state }))
    {
    }

    bool await_ready()
    {
        mState.start();
        return mFlag.test() && mHasValue;
    }

    bool await_suspend(std::coroutine_handle<CoroutineBehaviorState> behavior)
    {
        mBehavior = behavior;
        if (mFlag.test_and_set()) {
            if (mHasValue) {
                return false;
            } else if (mError) {
                mBehavior.promise().set_error(*mError);
                return true;
            } else {
                mBehavior.promise().set_done();
                return true;
            }
        } else {
            return true;
        }
    }

    void await_resume()
    {
        assert(mResult);
    }

    void set_value()
    {
        mResult = true;
        if (mFlag.test_and_set())
            mBehavior.resume();
    }

    void set_done()
    {
        if (mFlag.test_and_set())
            mBehavior.promise().set_done();
    }

    void set_error(R result)
    {
        mError = std::forward<R>(mError);
        if (mFlag.test_and_set())
            mBehavior.promise().set_error(std::forward<R>(*mError));
    }

private:
    S mState;
    std::atomic_flag mFlag = ATOMIC_FLAG_INIT;
    std::coroutine_handle<CoroutineBehaviorState> mBehavior;
    std::optional<R> mError;
    bool mResult = false;
};

template <typename Sender, typename V>
struct BehaviorAwaitableSenderImpl<Sender, V> {

    using S = Execution::connect_result_t<BehaviorTracker::sender<Sender>, BehaviorAwaitableReceiver<Sender, V>>;
    using R = typename Sender::result_type;

    BehaviorAwaitableSenderImpl(Sender &&sender, CoroutineBehaviorState *state)
        : mState(Execution::connect(std::forward<Sender>(sender) | state->mContext, BehaviorAwaitableReceiver<Sender, V> { {}, this, state }))
    {
    }

    bool await_ready()
    {
        mState.start();
        return mFlag.test() && mResult;
    }

    bool await_suspend(std::coroutine_handle<CoroutineBehaviorState> behavior)
    {
        mBehavior = behavior;
        if (mFlag.test_and_set()) {
            if (mResult) {
                return false;
            } else if (mError) {
                mBehavior.promise().set_error(std::forward<R>(*mError));
                return true;
            } else {
                mBehavior.promise().set_done();
                return true;
            }
        } else {
            return true;
        }
    }

    MakeOwning_t<V> await_resume()
    {
        return std::forward<MakeOwning_t<V>>(*mResult);
    }

    void set_value(V values)
    {
        mResult = { std::forward<V>(values) };
        if (mFlag.test_and_set())
            mBehavior.resume();
    }

    void set_done()
    {
        if (mFlag.test_and_set())
            mBehavior.promise().set_done();
    }

    void set_error(R result)
    {
        mError = std::forward<R>(result);
        if (mFlag.test_and_set())
            mBehavior.promise().set_error(std::forward<R>(*mError));
    }

private:
    S mState;
    std::atomic_flag mFlag = ATOMIC_FLAG_INIT;
    std::coroutine_handle<CoroutineBehaviorState> mBehavior;
    std::optional<R> mError;
    std::optional<MakeOwning_t<V>> mResult;
};

}
