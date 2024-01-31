#pragma once

#include "Generic/execution/virtualsender.h"
#include "Meta/keyvalue/argumentlist.h"
#include "behaviorerror.h"

#include "Meta/keyvalue/valuetype_forward.h"

#include "Generic/execution/state.h"

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
};

struct BehaviorReceiverPtr {

    template <typename... Args>
    void set_value(Args &&...args)
    {
        set_value(ArgumentList { std::forward<Args>(args)... });
    }
    void set_value(ArgumentList args)
    {
        mReceiver->set_value(std::move(args));
    }
    void set_error(BehaviorError r)
    {
        mReceiver->set_error(std::move(r));
    }
    void set_done()
    {
        mReceiver->set_done();
    }

    template <typename O>
    friend auto tag_invoke(Execution::resolve_var_d_t, BehaviorReceiverPtr &rec, std::string_view name, O &out)
    {
        ValueType v;
        if (rec.mReceiver->resolveVar(name, v)) {
            out = v.as<O>();
            return true;
        } else {
            return false;
        }
    }

    friend std::stop_token tag_invoke(Execution::get_stop_token_t, BehaviorReceiverPtr &rec)
    {
        return rec.mReceiver->stopToken();
    }

    friend Debug::ParentLocation *tag_invoke(Execution::get_debug_location_t, BehaviorReceiverPtr &rec)
    {
        return rec.mReceiver->debugLocation();
    }

    BehaviorReceiver *mReceiver;
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

}