#pragma once

#include "valuetype_forward.h"

#include "Interfaces/stringutil.h"
#include "functiontable.h"

namespace Engine {

struct MetaFunctionTag;

template <size_t Line>
using MetaFunctionLineStruct = LineStruct<MetaFunctionTag, Line>;

/*template <typename R, typename T>
static constexpr std::array<FunctionArgument, 0> metafunctionArgs(R (T::*f)(), const char *args)
{
    return { { { toValueTypeDesc<T*>() } } };
}*/

template <typename R, typename T, typename... Args, size_t... Is>
static constexpr std::array<FunctionArgument, sizeof...(Args) + 1> metafunctionArgsMemberHelper(std::string_view args, std::index_sequence<Is...>)
{
    std::array<std::string_view, sizeof...(Args)> argumentNames = StringUtil::tokenize<sizeof...(Args)>(args, ',');
    return { { { toValueTypeDesc<T *>(), "this" }, { toValueTypeDesc<std::remove_const_t<std::remove_reference_t<Args>>>(), argumentNames[Is] }... } };
}

template <typename R, typename T, typename... Args>
static constexpr std::array<FunctionArgument, sizeof...(Args) + 1> metafunctionArgs(R (T::*f)(Args...), std::string_view args)
{
    return metafunctionArgsMemberHelper<R, T, Args...>(args, std::make_index_sequence<sizeof...(Args)>());
}

template <auto F, typename R, typename T, typename... Args, size_t... I>
static void unpackMemberHelper(const FunctionTable *table, ValueType &retVal, const ArgumentList &args, std::index_sequence<I...>)
{
    T *t = ValueType_as<TypedScopePtr>(getArgument(args, 0)).safe_cast<T>();
    if constexpr (std::is_same_v<R, void>) {
        (t->*F)(ValueType_as<std::remove_cv_t<std::remove_reference_t<Args>>>(getArgument(args, I + 1))...);
        to_ValueType(retVal, std::monostate {});
    } else {
        to_ValueType(retVal, (t->*F)(ValueType_as<std::remove_cv_t<std::remove_reference_t<Args>>>(getArgument(args, I + 1))...));
    }
}

template <auto F, typename R, typename T, typename... Args>
static void unpackMemberApiMethod(const FunctionTable *table, ValueType &retVal, const ArgumentList &args)
{
    unpackMemberHelper<F, R, T, Args...>(table, retVal, args, std::make_index_sequence<sizeof...(Args)>());
}

template <auto F, typename R, typename T, typename... Args>
static constexpr typename FunctionTable::FPtr wrapHelper(R (T::*f)(Args...))
{
    return &unpackMemberApiMethod<F, R, T, Args...>;
}


}

#define FUNCTIONTABLE(F, ...)                                                      \
    namespace Engine {                                                             \
        template <>                                                                \
        struct LineStruct<MetaFunctionTag, __LINE__> {                             \
            static constexpr const auto args = metafunctionArgs(&F, #__VA_ARGS__); \
        };                                                                         \
    }                                                                              \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::FunctionTable, ::, function, SINGLE_ARG({ ::Engine::wrapHelper<&F>(&F), ::Engine::CallableTraits<decltype(&F)>::argument_count, ::Engine::CallableTraits<decltype(&F)>::is_member_function, ::Engine::MetaFunctionLineStruct<__LINE__>::args.data() }), &F);