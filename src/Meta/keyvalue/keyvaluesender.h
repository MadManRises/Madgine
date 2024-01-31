#pragma once

#include "Generic/execution/virtualstate.h"
#include "Generic/execution/concepts.h"

namespace Engine {

struct KeyValueSenderStateBase {
    virtual void connect(Execution::VirtualReceiverBase<GenericResult, const ArgumentList &> &receiver) = 0;
    virtual void start() = 0;
};

template <typename Sender>
struct KeyValueSenderState : KeyValueSenderStateBase {

    using State = Execution::connect_result_t<Sender, Execution::VirtualReceiverBase<GenericResult, const ArgumentList &> &>;

    KeyValueSenderState(Sender &&sender)
        : mState(std::forward<Sender>(sender))
    {
    }

    virtual void connect(Execution::VirtualReceiverBase<GenericResult, const ArgumentList &> &receiver) override
    {
        mState = Execution::connect(std::forward<Sender>(std::get<Sender>(mState)), receiver);
    }

    virtual void start() override
    {
        std::get<State>(mState).start();
    }    

    std::variant<Sender, State> mState;
};

struct KeyValueSender {

    using is_sender = void;

    template <Execution::Sender Sender>
    requires DecayedNoneOf<Sender, KeyValueSender>
    KeyValueSender(Sender &&sender)
        : mState(std::make_shared<KeyValueSenderState<Sender>>(std::forward<Sender>(sender)))
    {
    }

    KeyValueSender() = default;
    KeyValueSender(const KeyValueSender &) = default;
    KeyValueSender(KeyValueSender &&) = default;

    KeyValueSender &operator=(const KeyValueSender &) = default;
    KeyValueSender &operator=(KeyValueSender &&) = default;

    template <typename Rec>
    struct state : Execution::VirtualState<Rec, GenericResult, const ArgumentList &> {
        state(Rec &&rec, std::shared_ptr<KeyValueSenderStateBase> state)
            : Execution::VirtualState<Rec, GenericResult, const ArgumentList &>(std::forward<Rec>(rec))
            , mState(std::move(state))
        {
            mState->connect(*this);
        }

        void start()
        {
            mState->start();
        }

        std::shared_ptr<KeyValueSenderStateBase> mState;
    };

    template <typename Rec>
    friend auto tag_invoke(Execution::connect_t, const KeyValueSender &sender, Rec &&rec)
    {
        return state<Rec> { std::forward<Rec>(rec), sender.mState };
    }

private:
    std::shared_ptr<KeyValueSenderStateBase> mState;
};

}