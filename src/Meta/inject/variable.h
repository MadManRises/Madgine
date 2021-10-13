#pragma once

#include "context.h"

DERIVE_FUNCTION(notify);

namespace Engine {
namespace Inject {

    template <typename T, typename... Injectors>
    struct Variable;

    template <typename T, bool isClass>
    struct VariableBase : T {
    };

    template <typename T>
    struct VariableBase<T, false> {
        template <typename... Args>
        VariableBase(Args &&...args)
            : mData(std::forward<Args>(args)...)
        {
        }

        template <typename... Args>
        VariableBase(noContext, Args &&...args)
            : mData(std::forward<Args>(args)...)
        {
        }

        T mData;

        operator T& () {
            return mData;
        }
    };

    template <typename T>
    struct Variable<T> : VariableBase<T, std::is_class_v<T>> {
        using VariableBase<T, std::is_class_v<T>>::VariableBase;

        void notify() {

        }
    };

    template <typename T, typename Injector, typename... Injectors>
    struct Variable<T, Injector, Injectors...> : Variable<T, Injectors...> {
        template <typename... Args>
        Variable(Args &&...args)
            : Variable<T, Injectors...>(noContext {} , std::forward<Args>(args)...)
        {
            notify();
        }

        template <typename... Args>
        Variable(noContext, Args &&...args)
            : Variable<T, Injectors...>(noContext {}, std::forward<Args>(args)...)
        {
        }

        void notify() {
            if constexpr (has_function_notify_v<typename Injector::Context>)
                sContext()->notify();
            Variable<T, Injectors...>::notify();
        }

        std::shared_ptr<typename Injector::Context> sContext()
        {
            return get_context<Injector, Injectors...>(sGenericContext.lock());
        }

        void operator++() {
            ++static_cast<Variable<T, Injectors...>&>(*this);
            notify();
        }

        void operator--()
        {
            --static_cast<Variable<T, Injectors...> &>(*this);
            notify();
        }

    };

}
}