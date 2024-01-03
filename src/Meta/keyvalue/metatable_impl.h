#pragma once

#include "Generic/fixed_string.h"
#include "Generic/linestruct.h"
#include "Generic/type_holder.h"
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

            to_ValueType(retVal, forward_ref<T>(value));
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

}

#define METATABLE_BEGIN(T) METATABLE_BEGIN_BASE(T, void)

#define METATABLE_BEGIN_BASE(T, Base)                                        \
    namespace Engine {                                                       \
        template <>                                                          \
        struct LineStruct<MetaTableTag, __LINE__> {                          \
            using BaseT = Base;                                              \
            using Ty = T;                                                    \
            static constexpr const bool base = true;                         \
            constexpr const std::pair<const char *, Accessor> *data() const; \
            static constexpr const fixed_string name = #T;                   \
        };                                                                   \
        template <>                                                          \
        struct LineStruct<MetaTableCtorTag, __LINE__> {                      \
            using Ty = T;                                                    \
            constexpr const Constructor *data() const;                       \
            static constexpr const bool base = true;                         \
        };                                                                   \
    }

#define METATABLE_ENTRY(Name, Acc)                                          \
    namespace Engine {                                                      \
        LINE_STRUCT(MetaTableTag)                                           \
        {                                                                   \
            constexpr const std::pair<const char *, Accessor> *data() const \
            {                                                               \
                if constexpr (BASE_STRUCT(MetaTableTag)::base)              \
                    return &mData;                                          \
                else                                                        \
                    return BASE_STRUCT(MetaTableTag)::data();               \
            }                                                               \
            static constexpr const bool base = false;                       \
            std::pair<const char *, Accessor> mData = { Name, Acc };        \
        };                                                                  \
    }

#define CONSTRUCTOR(...)                                             \
    namespace Engine {                                               \
        LINE_STRUCT(MetaTableCtorTag)                                \
        {                                                            \
            constexpr const Constructor *data() const                \
            {                                                        \
                if constexpr (BASE_STRUCT(MetaTableCtorTag)::base)   \
                    return &mData;                                   \
                else                                                 \
                    return BASE_STRUCT(MetaTableCtorTag)::data();    \
            }                                                        \
            static constexpr const bool base = false;                \
            Constructor mData = ctorHelper<Ty>::ctor<__VA_ARGS__>(); \
        };                                                           \
    }

#define METATABLE_END(T)                                                                         \
    METATABLE_ENTRY(nullptr, SINGLE_ARG({ nullptr, nullptr }))                                   \
    CONSTRUCTOR(void)                                                                            \
    namespace Meta_##T                                                                           \
    {                                                                                            \
        static constexpr ::Engine::LineStruct<::Engine::MetaTableTag, __LINE__> sMembers = {};   \
        static constexpr ::Engine::LineStruct<::Engine::MetaTableCtorTag, __LINE__> sCtors = {}; \
    }                                                                                            \
    DLL_EXPORT_VARIABLE(constexpr, const ::Engine::MetaTable, , table, SINGLE_ARG({ #T, ::Engine::type_holder<T>, ::Engine::type_holder<Engine::MetaTableLineStruct<__LINE__>::BaseT>, Meta_##T::sMembers.data(), Meta_##T::sCtors.data() }), T);

#define NAMED_MEMBER(Name, M) \
    METATABLE_ENTRY(STRINGIFY(Name), SINGLE_ARG(::Engine::member<Ty, &Ty::M>()))

#define MEMBER(M) \
    NAMED_MEMBER(M, M)

#define READONLY_PROPERTY(Name, Getter) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, nullptr>()))

#define PROPERTY(Name, Getter, Setter) \
    METATABLE_ENTRY(#Name, SINGLE_ARG(::Engine::property<Ty, &Ty::Getter, &Ty::Setter>()))

#define NAMED_FUNCTION(Name, F, ...)                                                                                                                                                                                                               \
    FUNCTIONTABLE_EX(::Engine::MetaTableLineStruct<__LINE__ - 1>::name + "::" STRINGIFY(Name), ::Engine::MetaMemberFunctionTag<::Engine::MetaTableLineStruct<__LINE__ - 1>::Ty>, ::Engine::MetaTableLineStruct<__LINE__ - 1>::Ty::F, #__VA_ARGS__) \
    METATABLE_ENTRY(STRINGIFY(Name), SINGLE_ARG(::Engine::property<Ty, &::Engine::method<&Ty::F>, nullptr>()))

#define FUNCTION(F, ...) NAMED_FUNCTION(F, F, __VA_ARGS__)

#define PROXY(Getter) \
    READONLY_PROPERTY(__proxy, Getter)

#define CALL_OPERATOR(...) \
    NAMED_FUNCTION(__call, operator(), __VA_ARGS__)
