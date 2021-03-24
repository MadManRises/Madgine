#pragma once

#include "valuetype_forward.h"

namespace Engine {

struct META_EXPORT Function {
    template <typename R, typename... Args, size_t... I>
    static void unpackHelper(void (*f)(), ValueType &retVal, const ArgumentList &args, std::index_sequence<I...>)
    {
        if constexpr (std::is_same_v<R, void>) {
            reinterpret_cast<R (*)(Args...)>(f)(ValueType_as<std::remove_cv_t<std::remove_reference_t<Args>>>(getArgument(args, I))...);
            to_ValueType<true>(retVal, std::monostate {});
        } else {
            to_ValueType<true>(retVal, reinterpret_cast<R (*)(Args...)>(f)(ValueType_as<std::remove_cv_t<std::remove_reference_t<Args>>>(getArgument(args, I))...));
        }
    }

    template <typename R, typename... Args>
    static void unpackApiMethod(void (*f)(), ValueType &retVal, const ArgumentList &args)
    {
        unpackHelper<R, Args...>(f, retVal, args, std::make_index_sequence<sizeof...(Args)>());
    }

    template <typename R, typename... Args>
    Function(R (*f)(Args...))
        : mFunction(reinterpret_cast<void(*)()>(f))
        , mWrapper(&unpackApiMethod<R, Args...>)
    {
    }

    bool operator==(const Function& other) const {
        return mFunction == other.mFunction;
    }

    void operator()(ValueType &retVal, const ArgumentList &args) const;

private:
    void (*mFunction)();
    void (*mWrapper)(void (*)(), ValueType &, const ArgumentList &);
};

}