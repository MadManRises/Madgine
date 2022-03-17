#pragma once

#include "hierarchy/serializer.h"

#include "container/unithelper.h"

#include "Generic/memberoffsetptr.h"

#include "hierarchy/serializetable.h"

#include "operations.h"

#include "container/container_operations.h"

namespace Engine {
namespace Serialize {

    template <typename _disambiguate__dont_remove, auto P, auto Getter, auto Setter>
    constexpr Serializer encapsulated_pointer(const char *name)
    {
        using traits = CallableTraits<decltype(P)>;
        using Unit = typename traits::class_type;
        using T = std::decay_t<typename traits::return_type>;

        using getter_traits = CallableTraits<decltype(Getter)>;
        static_assert(std::same_as<Unit, std::remove_const_t<typename getter_traits::class_type>>);
        static_assert(std::same_as<T, typename getter_traits::return_type>);

        using setter_traits = CallableTraits<decltype(Setter)>;
        static_assert(std::same_as<Unit, typename setter_traits::class_type>);

        //TODO remove const in tuple types
        static_assert(std::same_as<typename setter_traits::argument_types, type_pack<T>>);

        return {
            name,
            []() {
                return OffsetPtr {};
            },
            [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write(out, (unit->*Getter)(), name, CallerHierarchyPtr { hierarchy.append(unit) });
            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                (unit->*Setter)(nullptr);
                return read(in, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
            },
            [](SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest *request) -> StreamResult {
                throw "Unsupported";
            },
            [](SyncableUnitBase *unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
                throw "Unsupported";
            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, bool success) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                STREAM_PROPAGATE_ERROR(applyMap(in, unit->*P, success));
                return {};
            },
            [](SerializableDataUnit *unit, bool b) {
            },
            [](SerializableDataUnit *_unit, bool active, bool existenceChanged) {
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
            [](SerializableDataUnit *unit) {
            },
            [](const SyncableUnitBase *unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                throw "Unsupported";
            },
            [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
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
        static_assert(std::same_as<Unit, std::decay_t<typename setter_traits::class_type>>);

        static_assert(std::same_as<typename setter_traits::decay_argument_types, type_pack<T>>);

        return {
            name,
            []() {
                return OffsetPtr {};
            },
            [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write(out, (unit->*Getter)(), name, hierarchy.append(unit));
            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                MakeOwning_t<T> dummy;
                STREAM_PROPAGATE_ERROR(read(in, dummy, name, CallerHierarchyPtr { hierarchy.append(unit) }));
                (unit->*Setter)(std::move(dummy));
                return {};
            },
            [](SyncableUnitBase *unit, FormattedBufferedStream &in, PendingRequest *request) -> StreamResult {
                throw "Unsupported";
            },
            [](SyncableUnitBase *unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
                throw "Unsupported";
            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, bool success) {
                return StreamResult {};
            },
            [](SerializableDataUnit *unit, bool b) {
            },
            [](SerializableDataUnit *unit, bool active, bool existenceChanged) {
            },
            [](SerializableDataUnit *unit) {
            },
            [](const SyncableUnitBase *unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                throw "Unsupported";
            },
            [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
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
                return MemberOffsetPtr<Unit, T> { P }.template offset<SerializableDataUnit>();
            },
            [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                write<T, Configs...>(out, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                return read<T, Configs...>(in, unit->*P, name, CallerHierarchyPtr { hierarchy.append(unit) });
            },
            [](SyncableUnitBase *_unit, FormattedBufferedStream &in, PendingRequest *request) -> StreamResult {
                if constexpr (std::derived_from<T, SyncableBase>) {
                    Unit *unit = static_cast<Unit *>(_unit);
                    return readAction<T, Configs...>(unit->*P, in, request, CallerHierarchyPtr { CallerHierarchy { unit } });
                } else
                    throw "Unsupported";
            },
            [](SyncableUnitBase *_unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {                
                if constexpr (std::derived_from<T, SyncableBase>) { 
                    Unit *unit = static_cast<Unit *>(_unit);
                    return readRequest<T, Configs...>(unit->*P, inout, id, CallerHierarchyPtr { CallerHierarchy { unit } });
                } else
                    throw "Unsupported";
            },
            [](SerializableDataUnit *unit, FormattedSerializeStream &in, bool success) {
                return applyMap<T, Configs...>(in, static_cast<Unit *>(unit)->*P, success);
            },
            [](SerializableDataUnit *unit, bool b) {
                setSynced<T, Configs...>(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableDataUnit *unit, bool active, bool existenceChanged) {
                setActive<T, Configs...>(static_cast<Unit *>(unit)->*P, active, existenceChanged);
            },
            [](SerializableDataUnit *unit) {
                if constexpr (std::derived_from<T, SerializableUnitBase>)
                    setParent<T, Configs...>(static_cast<Unit *>(unit)->*P, unit);
            },
            [](const SyncableUnitBase *_unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                if constexpr (std::derived_from<T, SyncableBase>) {
                    const Unit *unit = static_cast<const Unit *>(_unit);
                    writeAction<T, Configs...>(unit->*P, outStreams, data, CallerHierarchyPtr { CallerHierarchy { unit } });
                } else
                    throw "Unsupported";
            },
            [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
                if constexpr (std::derived_from<T, SyncableBase>) {
                    const Unit *unit = static_cast<const Unit *>(_unit);
                    writeRequest<T, Configs...>(unit->*P, out, data, CallerHierarchyPtr { CallerHierarchy { unit } });
                } else
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
        static_assert(std::derived_from<T, SyncableBase>);

        return {
            name,
            []() {
                return MemberOffsetPtr<Unit, T> { P }.template offset<SerializableDataUnit>();
            },
            [](const SerializableDataUnit *_unit, FormattedSerializeStream &out, const char *name, CallerHierarchyBasePtr hierarchy) {

            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, const char *name, CallerHierarchyBasePtr hierarchy) -> StreamResult {
                return {};
            },
            [](SyncableUnitBase *_unit, FormattedBufferedStream &in, PendingRequest *request) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                return readAction<T, Configs...>(unit->*P, in, request, CallerHierarchyPtr { CallerHierarchy { unit } });
            },
            [](SyncableUnitBase *_unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                return readRequest<T, Configs...>(unit->*P, inout, id, CallerHierarchyPtr { CallerHierarchy { unit } });
            },
            [](SerializableDataUnit *unit, FormattedSerializeStream &in, bool success) {
                return StreamResult {};
            },
            [](SerializableDataUnit *unit, bool b) {
                //UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableDataUnit *unit, bool active, bool existenceChanged) {
                //UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableDataUnit *unit) {
                //UnitHelper<T>::setItemParent(static_cast<Unit *>(unit)->*P, unit);
            },
            [](const SyncableUnitBase *_unit, const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &outStreams, const void *data) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                writeAction<T, Configs...>(unit->*P, outStreams, data, CallerHierarchyPtr { CallerHierarchy { unit } });
            },
            [](const SyncableUnitBase *_unit, FormattedBufferedStream &out, const void *data) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                writeRequest<T, Configs...>(unit->*P, out, data, CallerHierarchyPtr { CallerHierarchy { unit } });
            }
        };
    }

}
}

#define SERIALIZETABLE_BEGIN_IMPL(T, Base)                                                            \
    namespace Engine {                                                                                \
        namespace Serialize {                                                                         \
            namespace __serialize_impl__ {                                                            \
                template <>                                                                           \
                struct SerializeInstance<T> {                                                         \
                    using Ty = T;                                                                     \
                    static constexpr const ::Engine::Serialize::SerializeTable &(*baseType)() = Base; \
                    static constexpr const ::Engine::Serialize::Serializer fields[] = {

#define SERIALIZETABLE_INHERIT_BEGIN(T, Base) SERIALIZETABLE_BEGIN_IMPL(T, &serializeTable<Base>)
#define SERIALIZETABLE_BEGIN(T) SERIALIZETABLE_BEGIN_IMPL(T, nullptr)

#define SERIALIZETABLE_END(T) \
    {                         \
        nullptr               \
    }                         \
    }                         \
    ;                         \
    }                         \
    ;                         \
    }                         \
    }                         \
    }                         \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG({ #T, ::Engine::type_holder<T>, ::Engine::Serialize::__serialize_impl__::SerializeInstance<T>::baseType, ::Engine::Serialize::__serialize_impl__::SerializeInstance<T>::fields, std::derived_from<T, ::Engine::Serialize::TopLevelUnitBase> }), T);

#define FIELD(...) \
    ::Engine::Serialize::field<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))),

#define SYNC(...) \
    ::Engine::Serialize::sync<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))),

#define ENCAPSULATED_FIELD(Name, Getter, Setter) \
    ::Engine::Serialize::encapsulated_field<Ty, &Ty::Getter, &Ty::Setter>(#Name),

#define ENCAPSULATED_POINTER(Name, Getter, Setter) \
    ::Engine::Serialize::encapsulated_pointer<Ty, &Ty::Name, &Ty::Getter, &Ty::Setter>(#Name),
