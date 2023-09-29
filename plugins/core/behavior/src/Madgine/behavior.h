#pragma once

#include "Generic/genericresult.h"

#include "Generic/execution/virtualsender.h"

#include "Generic/execution/sender.h"

#include "Meta/keyvalue/keyvaluereceiver.h"

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Meta/keyvalue/virtualscope.h"

namespace Engine {

ENUM_BASE(InterpretResult, GenericResult);

struct VariableScope {
    virtual bool resolveVar(ValueType &ref, std::string_view name, bool recursive = true) = 0;
    virtual std::map<std::string_view, ValueType> variables() = 0;
    virtual VariableScope *parent() const { return nullptr; }
};

struct BehaviorStateBase : VirtualScopeBase<> {
    virtual ~BehaviorStateBase() = default;

    virtual std::string_view name() const = 0;

    virtual void interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, uint32_t flowIn) = 0;
};

struct MADGINE_BEHAVIOR_EXPORT Behavior : Serialize::SerializableDataUnit {
    Behavior() = default;
    Behavior(std::unique_ptr<BehaviorStateBase> state);

    Behavior &operator=(std::unique_ptr<BehaviorStateBase> state);

    std::string_view name() const;

    auto sender() &&
    {
        struct state : private Execution::VirtualReceiverBase<InterpretResult> {
            state(std::unique_ptr<BehaviorStateBase> state)
                : mState(std::move(state))
            {
            }

            void start()
            {
                mState->interpretImpl(*this, 0);
            }

            std::unique_ptr<BehaviorStateBase> mState;
        };
        return Execution::make_virtual_sender<state, InterpretResult>(std::move(mState));
    }

    std::unique_ptr<BehaviorStateBase> mState;
};

}

REGISTER_TYPE(Engine::BehaviorStateBase);