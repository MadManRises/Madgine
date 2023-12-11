#pragma once

#include "Generic/execution/concepts.h"

#include "Generic/execution/statedescriptor.h"

#include "Generic/callable_view.h"

namespace Engine {

struct BehaviorTrackerContext {

    BehaviorTrackerContext() = default;
    BehaviorTrackerContext(BehaviorTracker *tracker)
        : mTracker(tracker)
    {
    }
    BehaviorTrackerContext(BehaviorTrackerState *parent);

    BehaviorTracker *mTracker = nullptr;
    BehaviorTrackerState *mParent = nullptr;
};

struct get_behavior_context_t {

    template <typename T>
    requires(!tag_invocable<get_behavior_context_t, T &>)
        BehaviorTrackerContext
        operator()(T &) const
    {
        return {};
    }

    template <typename T>
    requires tag_invocable<get_behavior_context_t, T &>
    auto operator()(T &t) const
        noexcept(is_nothrow_tag_invocable_v<get_behavior_context_t, T &>)
            -> tag_invoke_result_t<get_behavior_context_t, T &>
    {
        return tag_invoke(*this, t);
    }
};

constexpr get_behavior_context_t get_behavior_context;

struct get_behavior_name_t {

    template <typename T>
    requires(!tag_invocable<get_behavior_name_t, const T &>)
        std::string
        operator()(const T &) const
    {
        return "<Unknown>";
    }

    template <typename T>
    requires tag_invocable<get_behavior_name_t, const T &>
    auto operator()(const T &t) const
        noexcept(is_nothrow_tag_invocable_v<get_behavior_name_t, const T &>)
            -> tag_invoke_result_t<get_behavior_name_t, const T &>
    {
        return tag_invoke(*this, t);
    }
};

constexpr get_behavior_name_t get_behavior_name;

struct BehaviorTrackerState {

    BehaviorTrackerState(BehaviorTrackerContext context)
        : mContext(context)
    {
    }

    virtual std::string name() const = 0;

    void stop()
    {
        throw 0;
    }

    virtual void visitStateImpl(CallableView<void(const Execution::StateDescriptor &)> visitor) = 0;

    template <typename F>
    void visitState(F &&f)
    {
        visitStateImpl(CallableView<void(const Execution::StateDescriptor &)> { f });
    }

    BehaviorTrackerContext mContext;
};

struct MADGINE_BEHAVIOR_EXPORT BehaviorTracker {

    template <typename Rec>
    struct receiver : Execution::algorithm_receiver<Rec> {

        template <typename... V>
        requires(requires { std::declval<Rec>().set_value(std::declval<V>()...); }) void set_value(V &&...value)
        {
            mState->mContext.mTracker->unregisterState(mState);
            this->mRec.set_value(std::forward<V>(value)...);
        }

        void set_done()
        {
            mState->mContext.mTracker->unregisterState(mState);
            this->mRec.set_done();
        }

        template <typename... R>
        void set_error(R &&...result)
        {
            mState->mContext.mTracker->unregisterState(mState);
            this->mRec.set_error(std::forward<R>(result)...);
        }

        friend BehaviorTrackerContext tag_invoke(get_behavior_context_t, receiver &rec)
        {
            return rec.mState;
        }

        BehaviorTrackerState *mState;
    };

    template <typename Inner, typename Rec>
    struct state : Execution::algorithm_state<Inner, receiver<Rec>>, BehaviorTrackerState {
        state(Inner &&inner, Rec &&rec, BehaviorTrackerContext context)
            : Execution::algorithm_state<Inner, receiver<Rec>> { std::forward<Inner>(inner), std::forward<Rec>(rec), this }
            , BehaviorTrackerState { context }
        {
        }

        void start()
        {
            if (mContext.mTracker)
                mContext.mTracker->registerState(this);
            Execution::algorithm_state<Inner, receiver<Rec>>::start();
        }

        void visitStateImpl(CallableView<void(const Execution::StateDescriptor &)> visitor) override
        {
            Execution::visit_state(this->mState, visitor);
        }

        virtual std::string name() const override
        {
            return get_behavior_name(this->mState);
        }
    };

    template <typename Inner>
    struct sender : Execution::algorithm_sender<Inner> {

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
        {
            return state<Inner, Rec> { std::forward<Inner>(sender.mInner), std::forward<Rec>(rec), sender.mContext };
        }

        Inner mInner;
        BehaviorTrackerContext mContext;
    };

    template <typename Inner>
    friend auto operator|(Inner &&inner, BehaviorTracker &tracker)
    {
        return sender<Inner> { {}, std::forward<Inner>(inner), &tracker };
    }

    struct AccessGuard {

        auto begin() const
        {
            return mData.begin();
        }
        auto end() const
        {
            return mData.end();
        }

        std::lock_guard<std::mutex> mGuard;
        const std::vector<BehaviorTrackerState *> &mData;
    };

    void registerState(BehaviorTrackerState *state);
    void unregisterState(BehaviorTrackerState *state);

    AccessGuard guarded() const;

private:
    mutable std::mutex mMutex;
    std::vector<BehaviorTrackerState *> mStates;
};

inline BehaviorTrackerContext::BehaviorTrackerContext(BehaviorTrackerState *parent)
    : mTracker(parent->mContext.mTracker)
    , mParent(parent)
{
}

template <typename Inner>
auto operator|(Inner &&inner, BehaviorTrackerContext context)
{
    return BehaviorTracker::sender<Inner> { {}, std::forward<Inner>(inner), context };
}

}