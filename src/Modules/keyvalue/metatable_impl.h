#pragma once

#include "../generic/virtualiterator.h"
#include "accessor.h"
#include "apifunction.h"
#include "keyvalueiterator.h"
#include "metatable.h"
#include "functiontable.h"
#include "typedscopeptr.h"
#include "valuetype_forward.h"

namespace Engine {

template <typename Scope, auto Getter, auto Setter>
constexpr Accessor property()
{
    using getter_traits = CallableTraits<decltype(Getter)>;
    using GetterScope = typename getter_traits::class_type;
    using T = typename getter_traits::return_type;

    void (*setter)(TypedScopePtr, const ValueType &) = nullptr;

    if constexpr (Setter != nullptr) {
        using setter_traits = CallableTraits<decltype(Setter)>;
        using SetterScope = typename setter_traits::class_type;

        //TODO remove const in tuple types
        //static_assert(std::is_same_v<typename setter_traits::argument_types, std::tuple<T>>);

        setter = [](TypedScopePtr scope, const ValueType &v) {
            if constexpr (std::is_same_v<SetterScope, void>) {
                using SetterScope = std::remove_pointer_t<std::tuple_element_t<0, typename setter_traits::argument_types>>;
                if constexpr (std::is_convertible_v<Scope &, SetterScope &>) {
                    TupleUnpacker::invoke(Setter, scope.safe_cast<Scope>(), ValueType_as<std::decay_t<T>>(v));
                } else {
                    TupleUnpacker::invoke(Setter, scope, ValueType_as<std::decay_t<T>>(v));
                }
            } else {
                static_assert(std::is_convertible_v<Scope &, SetterScope &>);
                TupleUnpacker::invoke(Setter, scope.safe_cast<Scope>(), ValueType_as<std::decay_t<T>>(v));
            }
        };
    }

    return {
        [](ValueType &retVal, TypedScopePtr scope) {
            T value = [=]() -> T {
                if constexpr (std::is_same_v<GetterScope, void>) {
                    using GetterScope = std::remove_pointer_t<std::tuple_element_t<0, typename getter_traits::argument_types>>;
                    if constexpr (std::is_convertible_v<Scope &, GetterScope &>) {
                        return TupleUnpacker::invoke(Getter, scope.safe_cast<Scope>());
                    } else {
                        return TupleUnpacker::invoke(Getter, scope);
                    }
                } else {
                    static_assert(std::is_convertible_v<Scope &, GetterScope &>);
                    return TupleUnpacker::invoke(Getter, scope.safe_cast<Scope>());
                }
            }();

            if constexpr (isValueType_v<std::decay_t<T>>) {
                to_ValueType(retVal, std::forward<T>(value));
            } else if constexpr (std::is_reference_v<T> && std::is_convertible_v<T, ScopeBase &>) {
                to_ValueType(retVal, &value);
            } else if constexpr (is_iterable<T>::value) {
                to_ValueType(retVal,
                    KeyValueVirtualIterator { std::forward<T>(value), type_holder<Functor<to_KeyValuePair<decltype(*std::declval<typename derive_iterator<T>::iterator>())>>> });
            } else {
                static_assert(dependent_bool<T, false>::value, "The provided type can not be converted to a ValueType");
            }
        },
        setter
    };
}

template <auto P, typename Scope, typename T>
void setField(Scope *s, const T &t)
{
    s->*P = t;
}

template <auto P, typename Scope, typename T>
T *getFieldPtr(Scope *s)
{
    return &(s->*P);
}

template <typename Scope, auto P>
constexpr Accessor member()
{
    using traits = CallableTraits<decltype(P)>;
    using DerivedScope = typename traits::class_type;
    using T = typename traits::return_type;

    if constexpr (std::is_reference_v<T> && std::is_convertible_v<T, ScopeBase &>) {
        return property<Scope, &getFieldPtr<P, DerivedScope, std::remove_reference_t<T>>, nullptr>();
    } else if constexpr (std::is_const_v<DerivedScope> || !std::is_assignable_v<T &, const T &> || (is_iterable_v<T> && !std::is_same_v<std::string, T>))
        return property<Scope, P, nullptr>();
    else
        return property<Scope, P, &setField<P, DerivedScope, std::remove_reference_t<T>>>();
}

template <auto F, typename R, typename T, typename... Args, size_t... I>
static void unpackHelper(ValueType &retVal, T *t, const ArgumentList &args, std::index_sequence<I...>)
{
    if constexpr (std::is_same_v<R, void>) {
        (t->*F)(ValueType_as<std::remove_cv_t<std::remove_reference_t<Args>>>(getArgument(args, I))...);
        to_ValueType(retVal, std::monostate {});
    } else {
        retVal = (t->*F)(ValueType_as<std::remove_cv_t<std::remove_reference_t<Args>>>(getArgument(args, I))...);
    }
}

template <auto F, typename R, typename T, typename... Args>
static void unpackApiMethod(ValueType &retVal, TypedScopePtr scope, const ArgumentList &args)
{
    T *t = scope.safe_cast<T>();
    return unpackHelper<F, R, T, Args...>(retVal, t, args, std::make_index_sequence<sizeof...(Args)>());
}

template <auto F, typename R, typename T, typename... Args>
static ApiFunction wrapHelper(R (T::*f)(Args...))
{
    return ApiFunction {
        &unpackApiMethod<F, R, T, Args...>,
        &function<F>()
    };
}

template <auto F>
static constexpr ApiFunction method(TypedScopePtr scope)
{
    return wrapHelper<F>(F);
}

/*template <typename T, size_t... Is>
static constexpr std::array<std::pair<const char *, ::Engine::Accessor>, std::tuple_size_v<T> + 1> structMembers(std::index_sequence<Is...>)
{
    return {

        { nullptr, nullptr }
    };
}*/
}

#define METATABLE_BEGIN(T) _METATABLE_BEGIN_IMPL(T, nullptr)

#define METATABLE_BEGIN_BASE(T, Base) _METATABLE_BEGIN_IMPL(T, &table<Base>)

#define _METATABLE_BEGIN_IMPL(T, BasePtr)                                        \
    namespace {                                                                  \
        namespace Meta_##T                                                       \
        {                                                                        \
            using Ty = T;                                                        \
            constexpr const ::Engine::MetaTable &(*baseClassGetter)() = BasePtr; \
            constexpr const std::pair<const char *, ::Engine::Accessor> members[] = {

#define METATABLE_END(T)              \
    {                                 \
        nullptr, { nullptr, nullptr } \
    }                                 \
    }                                 \
    ;                                 \
    }                                 \
    }                                 \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::MetaTable, ::, table, SINGLE_ARG3({ #T, Meta_##T::baseClassGetter, Meta_##T::members }), T);

/*#define STRUCT_METATABLE(T)                                                                                              \
    namespace {                                                                                                          \
        namespace Meta_##T                                                                                               \
        {                                                                                                                \
            constexpr const auto members = ::Engine::structMembers<T>(std::make_index_sequence<std::tuple_size_v<T>>()); \
        }                                                                                                                \
    }                                                                                                                    \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::MetaTable, ::, table, SINGLE_ARG3({ #T, nullptr, Meta_##T::members.data() }), T);
*/
#define MEMBER(M) \
    { #M, ::Engine::member<Ty, &Ty::M>() },

#define READONLY_PROPERTY(Name, Getter) \
    { #Name, ::Engine::property<Ty, &Ty::Getter, nullptr>() },

#define PROPERTY(Name, Getter, Setter) \
    { #Name, ::Engine::property<Ty, &Ty::Getter, &Ty::Setter>() },

#define FUNCTION(F) \
    { #F, ::Engine::property<Ty, &::Engine::method<&Ty::F>, nullptr>() },
