#pragma once

#include "../generic/container/virtualiterator.h"
#include "../generic/linestruct.h"
#include "accessor.h"
#include "apifunction.h"
#include "functiontable_impl.h"
#include "keyvalueiterator.h"
#include "metatable.h"
#include "typedscopeptr.h"
#include "valuetype_forward.h"

namespace Engine {

struct MetaTableTag;

template <size_t... Ids>
using MetaTableLineStruct = LineStruct<MetaTableTag, Ids...>;

template <typename Scope, auto Getter, auto Setter>
constexpr Accessor property()
{
    using getter_traits = CallableTraits<decltype(Getter)>;
    using GetterScope = typename getter_traits::class_type;
    using T = typename getter_traits::return_type;

    void (*setter)(const TypedScopePtr &, const ValueType &) = nullptr;

    if constexpr (Setter != nullptr) {
        using setter_traits = CallableTraits<decltype(Setter)>;
        using SetterScope = typename setter_traits::class_type;

        //TODO remove const in tuple types
        //static_assert(std::is_same_v<typename setter_traits::argument_types, std::tuple<T>>);

        setter = [](const TypedScopePtr &scope, const ValueType &v) {
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
        [](ValueType &retVal, const TypedScopePtr &scope) {
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

            to_ValueType<true>(retVal, std::forward<T>(value));
        },
        setter
    };
}

template <auto P, typename Scope, typename T>
void setField(Scope *s, const T &t)
{
    s->*P = t;
}

template <typename Scope, auto P>
constexpr Accessor member()
{
    using traits = CallableTraits<decltype(P)>;
    using DerivedScope = typename traits::class_type;
    using T = typename traits::return_type;

    if constexpr (std::is_const_v<DerivedScope> || !std::is_assignable_v<T &, const T &> || (is_iterable_v<T> && !std::is_same_v<std::string &, T>)) {
        return property<Scope, P, nullptr>();
    } else {
        return property<Scope, P, &setField<P, DerivedScope, std::remove_reference_t<T>>>();
    }
}

template <auto F>
static constexpr ApiFunction method(TypedScopePtr scope)
{
    return { &function<F>() };
}

/*template <typename T, size_t... Is>
static constexpr std::array<std::pair<const char *, ::Engine::Accessor>, std::tuple_size_v<T> + 1> structMembers(std::index_sequence<Is...>)
{
    return {

        { nullptr, nullptr }
    };
}*/
}

#define METATABLE_BEGIN(T) _METATABLE_BEGIN_IMPL(T, nullptr, __LINE__)

#define METATABLE_BEGIN_EX(T, ...) _METATABLE_BEGIN_IMPL(T, nullptr, __LINE__, __VA_ARGS__)

#define METATABLE_BEGIN_BASE(T, Base) _METATABLE_BEGIN_IMPL(T, &table<Base>, __LINE__)

#define _METATABLE_BEGIN_IMPL(T, BasePtr, ...)                               \
    namespace Engine {                                                       \
        template <>                                                          \
        struct LineStruct<MetaTableTag, __VA_ARGS__> {                       \
            using Ty = T;                                                    \
            static constexpr const MetaTable **baseClass = BasePtr;          \
            static constexpr const bool base = true;                         \
            constexpr const std::pair<const char *, Accessor> *data() const; \
        };                                                                   \
    }

#define METATABLE_ENTRY(Name, Acc, ...)                                                       \
    namespace Engine {                                                                        \
        template <>                                                                           \
        struct LineStruct<MetaTableTag, __VA_ARGS__> : MetaTableLineStruct<__VA_ARGS__ - 1> { \
            constexpr const std::pair<const char *, Accessor> *data() const                   \
            {                                                                                 \
                if constexpr (MetaTableLineStruct<__VA_ARGS__ - 1>::base)                     \
                    return &mData;                                                            \
                else                                                                          \
                    return MetaTableLineStruct<__VA_ARGS__ - 1>::data();                      \
            }                                                                                 \
            static constexpr const bool base = false;                                         \
            std::pair<const char *, Accessor> mData = { Name, Acc };                          \
        };                                                                                    \
    }

#define METATABLE_END(T) \
    _METATABLE_END_IMPL(T, __LINE__)

#define METATABLE_END_EX(T, ...) \
    _METATABLE_END_IMPL(T, __LINE__, __VA_ARGS__)

#define _METATABLE_END_IMPL(T, ...)                                                \
    METATABLE_ENTRY(nullptr, SINGLE_ARG({ nullptr, nullptr }), __VA_ARGS__)        \
    namespace Meta_##T                                                             \
    {                                                                              \
        static constexpr ::Engine::MetaTableLineStruct<__VA_ARGS__> sMembers = {}; \
    }                                                                              \
    DLL_EXPORT_VARIABLE(constexpr, const ::Engine::MetaTable, , table, SINGLE_ARG({ &::table<T>, #T, ::Engine::MetaTableLineStruct<__VA_ARGS__>::baseClass, Meta_##T::sMembers.data() }), T);

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
    METATABLE_ENTRY(#M, SINGLE_ARG(::Engine::member<Ty, &Ty::M>()), __LINE__)

#define READONLY_PROPERTY(Name, Getter) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, nullptr>()), __LINE__)

#define READONLY_PROPERTY_EX(Name, Getter, ...) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, nullptr>()), __LINE__, __VA_ARGS__)

#define PROPERTY(Name, Getter, Setter) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, &Ty::Setter>()), __LINE__)

#define NAMED_FUNCTION(Name, /*F, */...)                                        \
    FUNCTIONTABLE(::Engine::MetaTableLineStruct<__LINE__ - 1>::Ty::__VA_ARGS__) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &::Engine::method<&Ty::FIRST(__VA_ARGS__)>, nullptr>()), __LINE__)

#define NAMED_FUNCTION_EX1(Name, I, /*F, */...)                                                            \
    FUNCTIONTABLE(::Engine::MetaTableLineStruct<SINGLE_ARG(SINGLE_ARG(__LINE__, I - 1))>::Ty::__VA_ARGS__) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &::Engine::method<&Ty::FIRST(__VA_ARGS__)>, nullptr>()), __LINE__, I)

#define FUNCTION(/*F, */...) NAMED_FUNCTION(FIRST(__VA_ARGS__), __VA_ARGS__)

#define PROXY(Getter) \
    READONLY_PROPERTY(__proxy, Getter)
