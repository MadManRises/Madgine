#pragma once

#include "Generic/fixed_string.h"
#include "Generic/linestruct.h"
#include "accessor.h"
#include "boundapifunction.h"
#include "functiontable_impl.h"
#include "metatable.h"
#include "valuetype_forward.h"

namespace Engine {

struct MetaTableTag;
struct MetaTableCtorTag;
template <typename T>
struct MetaMemberFunctionTag;

template <size_t... Ids>
using MetaTableLineStruct = LineStruct<MetaTableTag, Ids...>;

template <size_t... Ids>
using MetaTableCtorLineStruct = LineStruct<MetaTableCtorTag, Ids...>;

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
            if constexpr (std::same_as<SetterScope, void>) {
                using SetterScope = std::remove_pointer_t<typename setter_traits::argument_types::template select<0>>;
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
                if constexpr (std::same_as<GetterScope, void>) {
                    using GetterScope = std::remove_pointer_t<typename getter_traits::argument_types::template select<0>>;
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
        setter,
        std::is_same_v<ValueType, std::decay_t<T>>
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
    using T = std::remove_reference_t<typename traits::return_type>;

    if constexpr (std::is_const_v<DerivedScope> || !std::is_assignable_v<T &, const T &> || (std::ranges::range<T> && !String<T>)) {
        return property<Scope, P, nullptr>();
    } else {
        return property<Scope, P, &setField<P, DerivedScope, T>>();
    }
}

template <auto F>
static constexpr BoundApiFunction method(TypedScopePtr scope)
{
    return { &function<F>(), scope };
}

template <typename T>
struct ctorHelper {
    template <typename... Args>
    static constexpr Constructor ctor()
    {
        if constexpr (std::is_same_v<type_pack<Args...>, type_pack<void>>)
            return nullptr;
        else {
            return []() {
                return OwnedScopePtr {
                    std::make_shared<ScopeWrapper<T>>()
                };
            };
        }
    }
};

/*template <typename T, size_t... Is>
static constexpr std::array<std::pair<const char *, ::Engine::Accessor>, std::tuple_size_v<T> + 1> structMembers(std::index_sequence<Is...>)
{
    return {

        { nullptr, nullptr }
    };
}*/
}

#define METATABLE_BEGIN(T) _METATABLE_BEGIN_IMPL(T, nullptr, nullptr)

#define METATABLE_BEGIN_BASE(T, Base) _METATABLE_BEGIN_IMPL(T, &table<Base>, SINGLE_ARG(&Engine::inheritance_offset<Base, T>))

#define _METATABLE_BEGIN_IMPL(T, BasePtr, BaseOffset)                   \
    namespace Meta_##T                                                       \
    {                                                                        \
        static constexpr const ::Engine::MetaTable **baseClass = BasePtr;              \
        static constexpr size_t (*baseOffset)() = BaseOffset;                \
    }                                                                        \
    namespace Engine {                                                       \
        template <>                                                          \
        struct LineStruct<MetaTableTag, __LINE__> {                       \
            using Ty = T;                                                    \
            static constexpr const bool base = true;                         \
            constexpr const std::pair<const char *, Accessor> *data() const; \
            static constexpr const fixed_string name = #T;                   \
        };                                                                   \
        template <>                                                          \
        struct LineStruct<MetaTableCtorTag, __LINE__> {                   \
            using Ty = T;                                                    \
            constexpr const Constructor *data() const;                       \
            static constexpr const bool base = true;                         \
        };                                                                   \
    }

#define METATABLE_ENTRY(Name, Acc)                                                       \
    namespace Engine {                                                                        \
        template <>                                                                           \
        struct LineStruct<MetaTableTag, __LINE__> : MetaTableLineStruct<__LINE__ - 1> { \
            constexpr const std::pair<const char *, Accessor> *data() const                   \
            {                                                                                 \
                if constexpr (MetaTableLineStruct<__LINE__ - 1>::base)                     \
                    return &mData;                                                            \
                else                                                                          \
                    return MetaTableLineStruct<__LINE__ - 1>::data();                      \
            }                                                                                 \
            static constexpr const bool base = false;                                         \
            std::pair<const char *, Accessor> mData = { Name, Acc };                          \
        };                                                                                    \
    }

#define CONSTRUCTOR(...)                                                                        \
    namespace Engine {                                                                          \
        template <>                                                                             \
        struct LineStruct<MetaTableCtorTag, __LINE__> : MetaTableCtorLineStruct<__LINE__ - 1> { \
            constexpr const Constructor *data() const                                           \
            {                                                                                   \
                if constexpr (MetaTableCtorLineStruct<__LINE__ - 1>::base)                      \
                    return &mData;                                                              \
                else                                                                            \
                    return MetaTableCtorLineStruct<__LINE__ - 1>::data();                       \
            }                                                                                   \
            static constexpr const bool base = false;                                           \
            Constructor mData = ctorHelper<Ty>::ctor<__VA_ARGS__>();                                  \
        };                                                                                      \
    }

#define METATABLE_END(T) \
    _METATABLE_END_IMPL(T)

#define _METATABLE_END_IMPL(T)                                                  \
    METATABLE_ENTRY(nullptr, SINGLE_ARG({ nullptr, nullptr }))          \
    CONSTRUCTOR(void)                                                                \
    namespace Meta_##T                                                               \
    {                                                                                \
        static constexpr ::Engine::MetaTableLineStruct<__LINE__> sMembers = {};   \
        static constexpr ::Engine::MetaTableCtorLineStruct<__LINE__> sCtors = {}; \
    }                                                                                \
    DLL_EXPORT_VARIABLE(constexpr, const ::Engine::MetaTable, , table, SINGLE_ARG({ &::table<T>, #T, ::Meta_##T::baseClass, ::Meta_##T::baseOffset, Meta_##T::sMembers.data(), Meta_##T::sCtors.data() }), T);

/*#define STRUCT_METATABLE(T)                                                                                              \
    namespace {                                                                                                          \
        namespace Meta_##T                                                                                               \
        {                                                                                                                \
            constexpr const auto members = ::Engine::structMembers<T>(std::make_index_sequence<std::tuple_size_v<T>>()); \
        }                                                                                                                \
    }                                                                                                                    \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::MetaTable, ::, table, SINGLE_ARG3({ #T, nullptr, Meta_##T::members.data() }), T);
*/

#define NAMED_MEMBER(Name, M) \
    METATABLE_ENTRY(STRINGIFY(Name), SINGLE_ARG(::Engine::member<Ty, &Ty::M>()))

#define MEMBER(M) \
    NAMED_MEMBER(M, M)

#define READONLY_PROPERTY(Name, Getter) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, nullptr>()))

#define PROPERTY(Name, Getter, Setter) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, &Ty::Setter>()))

#define NAMED_FUNCTION(Name, /*F, */...)                                                                                                                                                                                                         \
    FUNCTIONTABLE_EX(::Engine::MetaTableLineStruct<__LINE__ - 1>::name + "::" STRINGIFY(Name), ::Engine::MetaMemberFunctionTag<::Engine::MetaTableLineStruct<__LINE__ - 1>::Ty>, ::Engine::MetaTableLineStruct<__LINE__ - 1>::Ty::__VA_ARGS__) \
    METATABLE_ENTRY(STRINGIFY(Name), SINGLE_ARG(::Engine::property<Ty, &::Engine::method<&Ty::FIRST(__VA_ARGS__)>, nullptr>()))

#define FUNCTION(/*F, */...) NAMED_FUNCTION(FIRST(__VA_ARGS__), __VA_ARGS__)

#define PROXY(Getter) \
    READONLY_PROPERTY(__proxy, Getter)

#define CALL_OPERATOR(...) \
    NAMED_FUNCTION(__call, operator(), __VA_ARGS__)
