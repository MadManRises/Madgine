#pragma once

#include "Generic/execution/virtualsender.h"
#include "Generic/genericresult.h"
#include "argumentlist.h"

#include "valuetype_forward.h"

namespace Engine {

struct KeyValueReceiver : Execution::VirtualReceiverBase<GenericResult, ArgumentList> {
    template <typename... Args>
    void set_value(Args &&...args)
    {
        ArgumentList argList { sizeof...(args) };
        size_t i = 0;
        (to_ValueType(argList[i++], std::forward<Args>(args)), ...);

        static_cast<Execution::VirtualReceiverBase<GenericResult, ArgumentList> *>(this)->set_value(std::move(argList));
    }
};

template <typename F, typename... Args>
auto make_key_value_sender(F &&f, Args &&...args)
{
    return Execution::make_virtual_sender<Execution::SimpleState<F, std::tuple<Args...>, KeyValueReceiver>, GenericResult, ArgumentList>(std::forward<F>(f), std::tuple<Args...> { std::forward<Args>(args)... });
}

}