#pragma once

#include "../generic/callable_traits.h"
#include "../generic/offsetptr.h"
#include "../generic/tupleunpacker.h"
#include "Interfaces/macros.h"
#include "container/unithelper.h"
#include "serializetable.h"
#include "streams/serializestream.h"

namespace Engine {
namespace Serialize {

    template <typename _disambiguate__dont_remove, auto P, auto Getter, auto Setter, typename... Args>
    constexpr Serializer encapsulated_field(const char *name)
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
                out.write((unit->*Getter)(), name);
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name) {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::is_pointer_v<T>) {
                    (unit->*Setter)(nullptr);
                    in.read(unit->*P, name);
                } else {
                    T dummy;
                    in.read(dummy, name, TupleUnpacker::construct<Args>(unit)...);
                    (unit->*Setter)(std::move(dummy));
                }
            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
                throw "Unsupported";
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout) {
                throw "Unsupported";
            },
            [](SerializableUnitBase *_unit, const std::map<size_t, SerializableUnitBase *> &map) {
                if constexpr (std::is_pointer_v<T>) {
                    Unit *unit = static_cast<Unit *>(_unit);
                    T val = unit->*P;
                    unit->*P = nullptr;
                    UnitHelper<T>::applyMap(map, val);
                    (unit->*Setter)(val);
                }
            },
            [](SerializableUnitBase *unit, bool b) {
            },
            [](SerializableUnitBase *unit, bool b) {
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
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    return OffsetPtr<Unit, T> { P }.template offset<SerializableUnitBase, SyncableBase>();
                else
                    return size_t { 0 };
            },
            [](const SerializableUnitBase *_unit, SerializeOutStream &out, const char *name) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                out.write(unit->*P, name);
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in, const char *name) {
                Unit *unit = static_cast<Unit *>(_unit);
                in.read(unit->*P, name, TupleUnpacker::construct<Args>(unit)...);
            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readAction(in, TupleUnpacker::construct<Args>(static_cast<Unit *>(unit))...);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout) {
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readRequest(inout, TupleUnpacker::construct<Args>(static_cast<Unit *>(unit))...);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) {
                UnitHelper<T>::applyMap(map, static_cast<Unit *>(unit)->*P);
            },
            [](SerializableUnitBase *unit, bool b) {
                UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit, bool b) {
                UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, b);
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
                return OffsetPtr<Unit, T> { P }.template offset<SerializableUnitBase, SyncableBase>();
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
            [](SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) {
            },
            [](SerializableUnitBase *unit, bool b) {
                //UnitHelper<T>::setItemDataSynced(static_cast<Unit *>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit, bool b) {
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

#define SERIALIZETABLE_END(T)                                    \
    {                                                            \
        nullptr, { nullptr, nullptr, nullptr, nullptr, nullptr } \
    }                                                            \
    }                                                            \
    ;                                                            \
    }                                                            \
    ;                                                            \
    }                                                            \
    }                                                            \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG5({ #T, ::Engine::Serialize::__SerializeInstance<T>::baseType, ::Engine::Serialize::__SerializeInstance<T>::fields, std::is_base_of_v<::Engine::Serialize::TopLevelSerializableUnitBase, T> }), T);

//First argument M left out to prevent bug with empty __VA_ARGS__.
//That way it contains at least one item
#define FIELD(...) \
    { STRINGIFY2(FIRST(__VA_ARGS__)), ::Engine::Serialize::field<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))) },
//read:   #define FIELD(M, ...) { #M, ::Engine::Serialize::field<&Ty::M, __VA_ARGS__>(#M) },

#define SYNC(...) \
    { STRINGIFY2(FIRST(__VA_ARGS__)), ::Engine::Serialize::sync<Ty, &Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))) },

#define ENCAPSULATED_FIELD(Name, Getter, Setter)                                                   \
    {                                                                                              \
        #Name, ::Engine::Serialize::encapsulated_field<Ty, &Ty::Name, &Ty::Getter, &Ty::Setter>(#Name) \
    },
