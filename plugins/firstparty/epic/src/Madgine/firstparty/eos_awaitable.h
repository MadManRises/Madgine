#pragma once

#include "Generic/type_pack.h"

namespace Engine {
namespace FirstParty {

    template <auto f, typename R, typename ArgsTuple>
    struct EOS_AwaitableImpl {
        template <typename... Args>
        EOS_AwaitableImpl(Args &&...args)
            : mArgs(std::forward<Args>(args)...)
        {
        }

        static void callback(const R *result)
        {
            EOS_AwaitableImpl *self = static_cast<EOS_AwaitableImpl *>(result->ClientData);
            self->mResult = result;
            self->mHandle();
        }

        bool await_ready()
        {
            return false;
        }
        void await_suspend(Threading::TaskHandle handle)
        {
            mHandle = std::move(handle);
            TupleUnpacker::invokeFromTuple(
                [this]<typename... Args>(Args && ...args) {
                    f(std::forward<Args>(args)..., this, &EOS_AwaitableImpl::callback);
                },
                mArgs);
        }
        const R *await_resume()
        {
            return mResult;
        }

        ArgsTuple mArgs;
        const R *mResult = nullptr;
        Threading::TaskHandle mHandle;
    };

    template <typename R>
    R infoTypeHelper(void (*)(const R *));

    template <typename F>
    using InfoTypeHelper = decltype(infoTypeHelper(std::declval<F>()));

    template <auto f, typename /*R*/, typename /*T*/, typename... Args>
    using EOS_AwaitableHelper = EOS_AwaitableImpl<f, InfoTypeHelper<typename type_pack<Args...>::template select<sizeof...(Args)-1>>, typename type_pack<Args...>::template select_first_n<sizeof...(Args) - 2>::as_tuple>;

    template <auto f>
    using EOS_Awaitable = typename FunctionCapture<EOS_AwaitableHelper, f>::type;

}
}