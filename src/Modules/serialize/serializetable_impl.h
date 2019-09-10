#pragma once

#include "../generic/callable_traits.h"
#include "../generic/offsetptr.h"
#include "../generic/tupleunpacker.h"
#include "Interfaces/macros.h"
#include "serializetable.h"
#include "streams/serializestream.h"
#include "container/unithelper.h"

namespace Engine {
namespace Serialize {

    template <auto P, typename... Args>
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
            [](const SerializableUnitBase *_unit, SerializeOutStream &out) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                if constexpr (std::is_base_of_v<SerializableUnitBase, T>)
                    serializeTable<T>().writeState(&(unit->*P), out);
                else
                    out << unit->*P;
            },
            [](SerializableUnitBase *_unit, SerializeInStream &in) {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::is_base_of_v<SerializableUnitBase, T>)
                    serializeTable<T>().readState(&(unit->*P), in);
                else
                    in.read(unit->*P, TupleUnpacker::construct<Args>(unit)...);
            },
            [](SerializableUnitBase *unit, SerializeInStream &in) {
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readAction(in, TupleUnpacker::construct<Args>(static_cast<Unit*>(unit))...);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *unit, BufferedInOutStream &inout) {
                if constexpr (std::is_base_of_v<SyncableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readRequest(inout, TupleUnpacker::construct<Args>(static_cast<Unit*>(unit))...);
                else
                    throw "Unsupported";
            },
            [](SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) {
            },
            [](SerializableUnitBase *unit, bool b) {
                UnitHelper<T>::setItemDataSynced(static_cast<Unit*>(unit)->*P, b);
            },
            [](SerializableUnitBase *unit, bool b) {
                UnitHelper<T>::setItemActive(static_cast<Unit *>(unit)->*P, b);
            }
        };
    }

}
}



#define SERIALIZETABLE_BEGIN_IMPL(T, Base)\
    namespace Engine {                      \
        namespace Serialize {               \
            template <>                     \
            struct __SerializeInstance<T> { \
                using Ty = T;               \
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
    { STRINGIFY2(FIRST(__VA_ARGS__)), ::Engine::Serialize::field<&Ty::__VA_ARGS__>(STRINGIFY2(FIRST(__VA_ARGS__))) },
//read:   #define FIELD(M, ...) { #M, ::Engine::Serialize::field<&Ty::M, __VA_ARGS__>(#M) },
