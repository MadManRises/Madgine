#pragma once

#include "metatable.h"
#include "../generic/callable_traits.h"
#include "container_traits.h"
#include "../generic/tupleunpacker.h"
#include "valuetype.h"
#include "accessor.h"
#include "Interfaces/macros.h"

namespace Engine {

template <class T, typename = void>
struct is_typed_iterable : std::false_type {
};

template <class T>
struct is_typed_iterable<T, std::void_t<decltype(std::declval<T>().typedBegin()), decltype(std::declval<T>().typedEnd())>> : std::true_type {
};

template <auto Getter, auto Setter>
constexpr Accessor property()
{
    using getter_traits = CallableTraits<decltype(Getter)>;
    using Scope = typename getter_traits::class_type;
    using T = typename getter_traits::return_type;

    void (*setter)(ScopeBase *, ValueType) = nullptr;

    if constexpr (Setter != nullptr) {
        using setter_traits = CallableTraits<decltype(Setter)>;

        static_assert(std::is_same_v<setter_traits::argument_types, std::tuple<T>>);

        setter = [](ScopeBase *scope, ValueType v) {
            TupleUnpacker::invoke(Setter, static_cast<Scope *>(scope), v.as<T>());
        };
    }

    return {
        [](ScopeBase *scope) {
            T value = TupleUnpacker::invoke(Getter, static_cast<Scope *>(scope));

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
            }
        },
        setter
    };
}

template <auto P>
constexpr Accessor member()
{
    //TODO check const
    return property<P, nullptr>();
}

}

#define METATABLE_BEGIN(T) \
    namespace {            \
        namespace Meta_##T \
        {                  \
            using Ty = T;  \
            constexpr const std::pair<const char *, ::Engine::Accessor> members[] = {

#define METATABLE_END(T)                                          \
    {                                                             \
        nullptr, { nullptr, nullptr }                             \
    }                                                             \
    }                                                             \
    ;                                                             \
    }                                                             \
    }                                                             \
	DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::MetaTable, ::, table, SINGLE_ARG2({ #T, Meta_##T::members }), T); 

#define MEMBER(M) \
    { #M, ::Engine::member<&Ty::M>() },

#define READONLY_PROPERTY(Name, Getter) \
    { #Name, ::Engine::property<&Ty::Getter, nullptr>() },
