#pragma once

#include "Generic/execution/virtualsender.h"
#include "Meta/keyvalue/argumentlist.h"
#include "behaviorerror.h"

#include "Meta/keyvalue/valuetype_forward.h"

#include "state.h"

namespace Engine {

struct BehaviorReceiver : Execution::VirtualReceiverBase<BehaviorError, ArgumentList> {
    template <typename... Args>
    void set_value(Args &&...args)
    {
        static_cast<Execution::VirtualReceiverBase<BehaviorError, ArgumentList> *>(this)->set_value(ArgumentList { std::forward<Args>(args)... });
    }

    virtual bool resolveVar(std::string_view name, ValueType &out) = 0;
    virtual Debug::ParentLocation *debugLocation() = 0;
    virtual std::stop_token stopToken() = 0;

    template <typename O>
    friend auto tag_invoke(Execution::resolve_var_d_t, BehaviorReceiver &rec, std::string_view name, O &out)
    {
        ValueType v;
        if (rec.resolveVar(name, v)) {
            out = v.as<O>();
            return true;
        } else {
            return false;
        }
    }

    friend std::stop_token tag_invoke(Execution::get_stop_token_t, BehaviorReceiver &rec)
    {
        return rec.stopToken();
    }

    friend Debug::ParentLocation *tag_invoke(Execution::get_debug_location_t, BehaviorReceiver &rec)
    {
        return rec.debugLocation();
    }
};

template <typename Rec, typename Base = BehaviorReceiver>
struct VirtualBehaviorState : Execution::VirtualStateEx<Rec, Base, type_pack<BehaviorError>, ArgumentList> {

    using Execution::VirtualStateEx<Rec, Base, type_pack<BehaviorError>, ArgumentList>::VirtualStateEx;

    bool resolveVar(std::string_view name, ValueType &out) override
    {
        return Execution::resolve_var_d(this->mRec, name, out);
    }

    Debug::ParentLocation *debugLocation() override
    {
        return Execution::get_debug_location(this->mRec);
    }

    std::stop_token stopToken() override
    {
        return Execution::get_stop_token(this->mRec);
    }
};

template <typename F, typename... Args>
auto make_simple_behavior_sender(F &&f, Args&&... args) {
    return Execution::make_sender<BehaviorError, ArgumentList>(
        [args = std::tuple<Args...> { std::forward<Args>(args)... }, f { forward_capture<F>(std::forward<F>(f)) }]<typename Rec>(Rec &&rec) mutable {
            return TupleUnpacker::constructExpand<VirtualBehaviorState<Rec, Execution::SimpleState<F, std::tuple<Args...>, BehaviorReceiver>>>(std::forward<Rec>(rec), std::forward<F>(f), std::move(args));
        });
}

}