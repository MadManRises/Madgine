#pragma once

#include "../generic/offsetptr.h"
#include "serializer.h"
#include "serializetable.h"
#include "streams/operations.h"
#include "unithelper.h"

namespace Engine {
namespace Serialize {

    template <typename _disambiguate__dont_remove, auto P, auto Getter, auto Setter>
    constexpr Serializer encapsulated_pointer(const char *name)
    {
        using traits = CallableTraits<decltype(P)>;
        using Unit = typename traits::class_type;
        using T = std::decay_t<typename traits::return_type>;

        using getter_traits = CallableTraits<decltype(Getter)>;
        static_assert(std::is_same_v<Unit, std::remove_const_t<typename getter_traits::class_type>>);
        static_assert(std::is_same_v<T, typename getter_traits::return_type>);

        using setter_traits = CallableTraits<decltype(Setter)>;
        static_assert(std::is_same_v<Unit, typename setter_traits::class_type>);

        //TODO remove const in tuple types
        static_assert(std::is_same_v<typename setter_traits::argument_types, std::tuple<T>>);

        return {
            name,
            []() {
                return size_t { 0 };
            },
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write(out, (unit->*Getter)(), name);
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name) {
                Unit *unit = static_cast<Unit *>(_unit);
                (unit->*Setter)(nullptr);
                read(in, unit->*P, name);
            },
            [](SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) {
                throw "Unsupported";
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout, TransactionId id) {
                throw "Unsupported";
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in) {
                Unit *unit = static_cast<Unit *>(_unit);
                UnitHelper<T>::applyMap(in, unit->*P);
            },
            [](SerializableUnitBase *unit, bool b) {
            },
            [](SerializableUnitBase *_unit, bool active, bool existenceChanged) {
                Unit *unit = static_cast<Unit *>(_unit);
                if (active) {
                    T val = unit->*P;
                    unit->*P = nullptr;
                    (unit->*Setter)(val);
                } else {
                    T val = unit->*P;
                    (unit->*Setter)(nullptr);
                    unit->*P = val;
                }
            }
        };
    }

    template <typename _disambiguate__dont_remove, auto Getter, auto Setter>
    constexpr Serializer encapsulated_field(const char *name)
    {

        using getter_traits = CallableTraits<decltype(Getter)>;
        using Unit = std::decay_t<typename getter_traits::class_type>;
        using T = std::decay_t<typename getter_traits::return_type>;

        using setter_traits = CallableTraits<decltype(Setter)>;
        static_assert(std::is_same_v<Unit, std::decay_t<typename setter_traits::class_type>>);

        static_assert(std::is_same_v<typename setter_traits::decay_argument_types, std::tuple<T>>);

        return {
            name,
            []() {
                return size_t { 0 };
            },
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write(out, (unit->*Getter)(), name);
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name) {
                Unit *unit = static_cast<Unit *>(_unit);
                T dummy;
                read(in, dummy, name);
                (unit->*Setter)(std::move(dummy));
            },
            [](SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) {
                throw "Unsupported";
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout, TransactionId id) {
                throw "Unsupported";
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in) {
            },
            [](SerializableUnitBase *unit, bool b) {
            },
            [](SerializableUnitBase *unit, bool active, bool existenceChanged) {
            }
        };
    }

    template <typename _disambiguate__dont_remove, auto P, typename... Args>
    constexpr Serializer field(const char *name)
    {
        using traits = CallableTraits<decltype(P)>;
        using Unit = typename traits::class_type;
        using T = std::decay_t<typename traits::return_type>;

        return {
            name,
            []() {
                return OffsetPtr<Unit, T> { P }.template offset<SerializableUnitBase>();
            },
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write<T, Args...>(out, unit->*P, name, unit);
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name) {
                Unit *unit = static_cast<Unit *>(_unit);
                read<T, Args...>(in, unit->*P, name, unit);
            },
            [](SerializableUnitBase *unit, SerializeInStream &in, PendingRequest *request) {
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readAction(in, request);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout, TransactionId id) {
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readRequest(inout, id);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
                UnitHelper<T>::applyMap(in, static_cast<Unit *>(unit)->*P);
            },
            [](SerializableUnitBase *unit, bool b) {
                UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit, bool active, bool existenceChanged) {
                UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, active, existenceChanged);
            }
        };
    }

    template <typename _disambiguate__dont_remove, auto P>
    constexpr Serializer sync(const char *name)
    {
        using traits = CallableTraits<decltype(P)>;
        using Unit = typename traits::class_type;
        using T = std::decay_t<typename traits::return_type>;
        static_assert(std::is_base_of_v<SyncableBase, T>);

        return {
            name,
            []() {
                return OffsetPtr<Unit, T> { P }.template offset<SerializableUnitBase>();
            },
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name) {

            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name) {

            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
                (static_cast<Unit *>(unit)->*P).readAction(in);
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout) {
                (static_cast<Unit *>(unit)->*P).readRequest(inout);
            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
            },
            [](SerializableUnitBase *unit, bool b) {
                //UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit, bool active, bool existenceChanged) {
                //UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, b);
            }
        };
    }

}
}

#define SERIALIZETABLE_BEGIN_IMPL(T, Base)                                                        \
    namespace Engine {                                                                            \
        namespace Serialize {                                                                     \
            template <>                                                                           \
            struct __SerializeInstance<T> {                                                       \
                using Ty = T;                                                                     \
                static constexpr const ::Engine::Serialize::SerializeTable &(*baseType)() = Base; \
                static constexpr const std::pair<const char *, ::Engine::Serialize::Serializer> fields[] = {

#define SERIALIZETABLE_INHERIT_BEGIN(T, Base) SERIALIZETABLE_BEGIN_IMPL(T, &serializeTable<Base>)
#define SERIALIZETABLE_BEGIN(T) SERIALIZETABLE_BEGIN_IMPL(T, nullptr)

#define SERIALIZETABLE_END(T) \
    {                         \
        nullptr, { nullptr }  \
    }                         \
    }                         \
    ;                         \
    }                         \
    ;                         \
    }                         \
    }                         \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG({ #T, ::Engine::type_holder<T>, ::Engine::Serialize::__SerializeInstance<T>::baseType, ::Engine::Serialize::__SerializeInstance<T>::fields, std::is_base_of_v<::Engine::Serialize::TopLevelSerializableUnitBase, T> }), T);

#define FIELD(...) \
    { STRINGIFY2(FIRST(__VA_ARGS__)), ::Engine::Serialize::field<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))) },

#define SYNC(...) \
    { STRINGIFY2(FIRST(__VA_ARGS__)), ::Engine::Serialize::sync<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))) },

#define ENCAPSULATED_FIELD(Name, Getter, Setter)                                            \
    {                                                                                       \
        #Name, ::Engine::Serialize::encapsulated_field<Ty, &Ty::Getter, &Ty::Setter>(#Name) \
    },

#define ENCAPSULATED_POINTER(Name, Getter, Setter)                                                       \
    {                                                                                                    \
        #Name, ::Engine::Serialize::encapsulated_pointer<Ty, &Ty::Name, &Ty::Getter, &Ty::Setter>(#Name) \
    },
