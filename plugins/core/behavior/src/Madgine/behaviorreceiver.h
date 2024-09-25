#pragma once

#include "Generic/execution/virtualsender.h"
#include "Meta/keyvalue/argumentlist.h"
#include "behaviorerror.h"

#include "Meta/keyvalue/valuetype_forward.h"

#include "bindings.h"

#include "Madgine/debug/debuggablesender.h"

namespace Engine {

struct MADGINE_BEHAVIOR_EXPORT BehaviorReceiver : Execution::VirtualReceiverBase<BehaviorError, ArgumentList> {
    template <typename... Args>
    void set_value(Args &&...args)
    {
        static_cast<Execution::VirtualReceiverBase<BehaviorError, ArgumentList> *>(this)->set_value(ArgumentList { std::forward<Args>(args)... });
    }

    virtual bool getBinding(std::string_view name, ValueType &out) = 0;
    virtual Debug::ParentLocation *debugLocation() = 0;
    virtual std::stop_token stopToken() = 0;
    virtual Log::Log *log() = 0;

    bool getBindingHelper(std::string_view name, void (*)(const ValueType &, void *), void *data);

    template <typename O>
    friend auto tag_invoke(get_binding_d_t, BehaviorReceiver &rec, std::string_view name, O &out)
    {
        if constexpr (std::same_as<O, ValueType> || std::same_as<O, ValueTypeRef>) {
            return rec.getBinding(name, out);
        } else {
            return rec.getBindingHelper(
                name, [](const ValueType &v, void *out) {
                    *static_cast<O *>(out) = ValueType_as<O>(v);
                },
                &out);
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

    friend Log::Log* tag_invoke(Log::get_log_t, BehaviorReceiver& rec)
    {
        return rec.log();
    }

};

template <typename Rec, typename Base = BehaviorReceiver>
struct VirtualBehaviorState : Execution::VirtualStateEx<Rec, Base, type_pack<BehaviorError>, ArgumentList> {

    using Execution::VirtualStateEx<Rec, Base, type_pack<BehaviorError>, ArgumentList>::VirtualStateEx;

    bool getBinding(std::string_view name, ValueType &out) override
    {
        ValueTypeRef outRef { out };
        return get_binding_d(this->mRec, name, outRef);
    }

    Debug::ParentLocation *debugLocation() override
    {
        return Execution::get_debug_location(this->mRec);
    }

    std::stop_token stopToken() override
    {
        return Execution::get_stop_token(this->mRec);
    }

    Log::Log* log() override
    {
        return Log::get_log(this->mRec);
    }
};

template <typename F, typename... Args>
auto make_simple_behavior_sender(F &&f, Args &&...args)
{
    return Execution::make_sender<BehaviorError, ArgumentList>(
        [args = std::tuple<Args...> { std::forward<Args>(args)... }, f { forward_capture<F>(std::forward<F>(f)) }]<typename Rec>(Rec &&rec) mutable {
            return TupleUnpacker::constructExpand<VirtualBehaviorState<Rec, Execution::SimpleState<F, std::tuple<Args...>, BehaviorReceiver>>>(std::forward<Rec>(rec), std::forward<F>(f), std::move(args));
        });
}

}