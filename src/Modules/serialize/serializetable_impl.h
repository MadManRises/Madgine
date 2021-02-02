#pragma once

#include "../generic/offsetptr.h"
#include "serializer.h"
#include "serializetable.h"
#include "streams/operations.h"
#include "unithelper.h"
#include "../generic/makeowning.h"

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
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                    write(out, (unit->*Getter)(), name, CallerHierarchyPtr { hierarchy.append(unit) } );
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy) {
                Unit *unit = static_cast<Unit *>(_unit);
                (unit->*Setter)(nullptr);
                read(in, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) } );
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
            },
            [](SerializableUnitBase *unit) {
            },
            [](const SerializableUnitBase *unit, int op, const void *data, const std::set<BufferedOutStream *, CompareStreamId> &outStreams) {
                throw "Unsupported";
            },
            [](const SerializableUnitBase *_unit, int op, const void *data, BufferedOutStream *out) {
                throw "Unsupported";
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
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write(out, (unit->*Getter)(), name, hierarchy.append(unit));
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy) {
                Unit *unit = static_cast<Unit *>(_unit);
                MakeOwning_t<T> dummy;
                read(in, dummy, name, CallerHierarchyPtr { hierarchy.append(unit) });
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
            },
            [](SerializableUnitBase *unit) {
            },
            [](const SerializableUnitBase *unit, int op, const void *data, const std::set<BufferedOutStream *, CompareStreamId> &outStreams) {
                throw "Unsupported";
            },
            [](const SerializableUnitBase *_unit, int op, const void *data, BufferedOutStream *out) {
                throw "Unsupported";
            }
        };
    }

    template <typename _disambiguate__dont_remove, auto P, typename... Configs>
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
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write<T, Configs...>(out, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name, CallerHierarchyBasePtr hierarchy) {
                Unit *unit = static_cast<Unit *>(_unit);
                read<T, Configs...>(in, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, PendingRequest *request) {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    Operations<T, Configs...>::readAction(unit->*P, in, request, unit);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *_unit, BufferedInOutStream &inout, TransactionId id) {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    Operations<T, Configs...>::readRequest(unit->*P, inout, id, unit);
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
            },
            [](SerializableUnitBase *unit) {
                UnitHelper<T>::setItemParent(static_cast<Unit *>(unit)->*P, unit);
            },
            [](const SerializableUnitBase *_unit, int op, const void *data, const std::set<BufferedOutStream *, CompareStreamId> &outStreams) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    Operations<T, Configs...>::writeAction(unit->*P, op, data, outStreams, unit);
                else
                    throw "Unsupported";
            },
            [](const SerializableUnitBase *_unit, int op, const void *data, BufferedOutStream *out) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    Operations<T, Configs...>::writeRequest(unit->*P, op, data, out, unit);
                else
                    throw "Unsupported";
            }
        };
    }

    template <typename _disambiguate__dont_remove, auto P, typename... Configs>
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
            [](SerializableUnitBase *_unit, SerializeInStream &in, PendingRequest *request) {
                Unit *unit = static_cast<Unit *>(_unit);
                Operations<T, Configs...>::readAction(unit->*P, in, request, unit);
            },
            [](SerializableUnitBase *_unit, BufferedInOutStream &inout, TransactionId id) {
                Unit *unit = static_cast<Unit *>(_unit);
                Operations<T, Configs...>::readRequest(unit->*P, inout, id, unit);
            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
            },
            [](SerializableUnitBase *unit, bool b) {
                //UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit, bool active, bool existenceChanged) {
                //UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit) {
                //UnitHelper<T>::setItemParent(static_cast<Unit *>(unit)->*P, unit);
            },
            [](const SerializableUnitBase *_unit, int op, const void *data, const std::set<BufferedOutStream *, CompareStreamId> &outStreams) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                Operations<T, Configs...>::writeAction(unit->*P, op, data, outStreams, unit);
            },
            [](const SerializableUnitBase *_unit, int op, const void *data, BufferedOutStream *out) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                Operations<T, Configs...>::writeRequest(unit->*P, op, data, out, unit);
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
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG({ #T, ::Engine::type_holder<T>, ::Engine::Serialize::__SerializeInstance<T>::baseType, ::Engine::Serialize::__SerializeInstance<T>::fields, std::is_base_of_v<::Engine::Serialize::TopLevelUnitBase, T> }), T);

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
