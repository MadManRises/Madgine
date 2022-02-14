#pragma once

#include "Generic/makeowning.h"
#include "Generic/memberoffsetptr.h"
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
        static_assert(std::is_same_v<typename setter_traits::argument_types, type_pack<T>>);

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
            [](SerializableDataUnit *unit, FormattedSerializeStream &in, PendingRequest *request) -> StreamResult {
                throw "Unsupported";
            },
            [](SerializableDataUnit *unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
                throw "Unsupported";
            },
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, bool success) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                STREAM_PROPAGATE_ERROR(UnitHelper<T>::applyMap(in, unit->*P, success));
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
            [](const SerializableDataUnit *unit, const std::set<FormattedBufferedStream *, CompareStreamId> &outStreams, const void *data) {
                throw "Unsupported";
            },
            [](const SerializableDataUnit *_unit, FormattedBufferedStream &out, const void *data) {
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

        static_assert(std::is_same_v<typename setter_traits::decay_argument_types, type_pack<T>>);

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
            [](SerializableDataUnit *unit, FormattedSerializeStream &in, PendingRequest *request) -> StreamResult {
                throw "Unsupported";
            },
            [](SerializableDataUnit *unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
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
            [](const SerializableDataUnit *unit, const std::set<FormattedBufferedStream *, CompareStreamId> &outStreams, const void *data) {
                throw "Unsupported";
            },
            [](const SerializableDataUnit *_unit, FormattedBufferedStream &out, const void *data) {
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
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, PendingRequest *request) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::derived_from<T, SyncableBase>)
                    return Operations<T, Configs...>::readAction(unit->*P, in, request, unit);
                else
                    throw "Unsupported";
            },
            [](SerializableDataUnit *_unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::derived_from<T, SyncableBase>)
                    return Operations<T, Configs...>::readRequest(unit->*P, inout, id, unit);
                else
                    throw "Unsupported";
            },
            [](SerializableDataUnit *unit, FormattedSerializeStream &in, bool success) {
                return UnitHelper<T>::applyMap(in, static_cast<Unit *>(unit)->*P, success);
            },
            [](SerializableDataUnit *unit, bool b) {
                UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableDataUnit *unit, bool active, bool existenceChanged) {
                UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, active, existenceChanged);
            },
            [](SerializableDataUnit *unit) {
                if constexpr (std::derived_from<T, SerializableUnitBase>)
                    UnitHelper<T>::setItemParent(static_cast<Unit *>(unit)->*P, unit);
            },
            [](const SerializableDataUnit *_unit, const std::set<FormattedBufferedStream *, CompareStreamId> &outStreams, const void *data) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                if constexpr (std::derived_from<T, SyncableBase>)
                    Operations<T, Configs...>::writeAction(unit->*P, outStreams, data, unit);
                else
                    throw "Unsupported";
            },
            [](const SerializableDataUnit *_unit, FormattedBufferedStream &out, const void *data) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                if constexpr (std::derived_from<T, SyncableBase>)
                    Operations<T, Configs...>::writeRequest(unit->*P, out, data, unit);
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
            [](SerializableDataUnit *_unit, FormattedSerializeStream &in, PendingRequest *request) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                return Operations<T, Configs...>::readAction(unit->*P, in, request, unit);
            },
            [](SerializableDataUnit *_unit, FormattedBufferedStream &inout, TransactionId id) -> StreamResult {
                Unit *unit = static_cast<Unit *>(_unit);
                return Operations<T, Configs...>::readRequest(unit->*P, inout, id, unit);
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
            [](const SerializableDataUnit *_unit, const std::set<FormattedBufferedStream *, CompareStreamId> &outStreams, const void *data) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                Operations<T, Configs...>::writeAction(unit->*P, outStreams, data, unit);
            },
            [](const SerializableDataUnit *_unit, FormattedBufferedStream &out, const void *data) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                Operations<T, Configs...>::writeRequest(unit->*P, out, data, unit);
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
                struct SerializeInstance<T> {                                                       \
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
    }                         \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG({ #T, ::Engine::type_holder<T>, ::Engine::Serialize::__serialize_impl__::SerializeInstance<T>::baseType, ::Engine::Serialize::__serialize_impl__::SerializeInstance<T>::fields, std::derived_from<T, ::Engine::Serialize::TopLevelUnitBase> }), T);

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
