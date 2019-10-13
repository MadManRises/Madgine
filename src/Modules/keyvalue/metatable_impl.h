#pragma once

#include "../generic/callable_traits.h"
#include "../generic/tupleunpacker.h"
#include "../reflection/decay.h"
#include "Interfaces/macros.h"
#include "accessor.h"
#include "container_traits.h"
#include "metatable.h"
#include "valuetype.h"

namespace Engine {

template <class T, typename = void>
struct is_typed_iterable : std::false_type {
};

template <class T>
struct is_typed_iterable<T, std::void_t<decltype(std::declval<T>().typedBegin()), decltype(std::declval<T>().typedEnd())>> : std::true_type {
};

template <typename _disambiguate__dont_remove, auto Getter, auto Setter>
constexpr Accessor property()
{
    using getter_traits = CallableTraits<decltype(Getter)>;
    using Scope = typename getter_traits::class_type;
    using T = typename getter_traits::return_type;

    void (*setter)(TypedScopePtr, ValueType) = nullptr;

    if constexpr (Setter != nullptr) {
        using setter_traits = CallableTraits<decltype(Setter)>;
        using SetterScope = std::conditional_t<std::is_same_v<typename setter_traits::class_type, void>, std::remove_pointer_t<std::tuple_element_t<0, typename setter_traits::argument_types>>, typename setter_traits::class_type>;

        //TODO remove const in tuple types
        //static_assert(std::is_same_v<typename setter_traits::argument_types, std::tuple<T>>);

        setter = [](TypedScopePtr scope, ValueType v) {
            TupleUnpacker::invoke(Setter, scope.safe_cast<decayed_t<SetterScope>>(), v.as<std::remove_reference_t<T>>());
        };
    }

    return {
        [](TypedScopePtr scope) {
            T value = [=]() -> T {
                if constexpr (std::is_same_v<Scope, void>)
                    return TupleUnpacker::invoke(Getter, scope);
                else
                    return TupleUnpacker::invoke(Getter, scope.safe_cast<decayed_t<Scope>>());
            }();

            if constexpr (ValueType::isValueType<T>::value) {
                return ValueType { std::forward<T>(value) };
            } else if constexpr (is_typed_iterable<T>::value) {
                return ValueType {
                    KeyValueVirtualIterator { KeyValueIterator { value.typedBegin() }, KeyValueIterator { value.typedEnd() } }
                };
            } else if constexpr (is_iterable<T>::value) {
                return ValueType {
                    KeyValueVirtualIterator { KeyValueIterator { value.begin() }, KeyValueIterator { value.end() } }
                };
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

template <typename Ty, auto P>
constexpr Accessor member()
{
    using traits = CallableTraits<decltype(P)>;
    using Scope = typename traits::class_type;
    using T = std::remove_reference_t<typename traits::return_type>;

    if constexpr (std::is_const_v<Scope> || !std::is_assignable_v<T &, const T &> || (is_iterable_v<T> && !std::is_same_v<std::string, T>))
        return property<Ty, P, nullptr>();
    else
        return property<Ty, P, &setField<P, Scope,T>>();
}

template <auto F, typename R, typename T, typename... Args, size_t... I>
static ValueType unpackHelper(T *t, const ArgumentList &args, std::index_sequence<I...>)
{
    if constexpr (std::is_same_v<R, void>) {
        (t->*F)(args.at(I).as<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
        return {};
    } else {
        return (t->*F)(args.at(I).as<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
    }
}

template <auto F, typename R, typename T, typename... Args>
static ValueType unpackApiMethod(TypedScopePtr scope, const ArgumentList &args)
{
    T *t = scope.safe_cast<T>();
    return unpackHelper<F, R, T, Args...>(t, args, std::make_index_sequence<sizeof...(Args)>());
}

template <auto F, typename R, typename T, typename... Args>
static ApiMethod wrapHelper(R (T::*f)(Args...))
{
    return ApiMethod {
        &unpackApiMethod<F, R, T, Args...>,
        sizeof...(Args),
        false
    };
}

template <auto F>
static constexpr BoundApiMethod method(TypedScopePtr scope)
{
    return { wrapHelper<F>(F), scope };
}

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

#define MEMBER(M) \
    { #M, ::Engine::member<Ty, &Ty::M>() },

#define READONLY_PROPERTY(Name, Getter) \
    { #Name, ::Engine::property<Ty, &Ty::Getter, nullptr>() },

#define PROPERTY(Name, Getter, Setter) \
    { #Name, ::Engine::property<Ty, &Ty::Getter, &Ty::Setter>() },

#define FUNCTION(F) \
    { #F, ::Engine::property<Ty, &::Engine::method<&Ty::F>, nullptr>() },
