#pragma once

#include "Interfaces/macros.h"
#include "serializetable.h"
#include "../generic/callable_traits.h"
#include "../generic/offsetptr.h"
#include "streams/serializestream.h"
#include "../generic/tupleunpacker.h"

namespace Engine {
namespace Serialize {

	template <auto P, typename... Args>
    constexpr Serializer field()
    {
        using traits = CallableTraits<decltype(P)>;
        using Unit = typename traits::class_type;
        using T = std::decay_t<typename traits::return_type>;

        /*void (*setter)(TypedScopePtr, ValueType) = nullptr;

        if constexpr (Setter != nullptr) {
            using setter_traits = CallableTraits<decltype(Setter)>;

            static_assert(std::is_same_v<typename setter_traits::argument_types, std::tuple<T>>);

            setter = [](TypedScopePtr scope, ValueType v) {
                TupleUnpacker::invoke(Setter, scope.safe_cast<Scope>(), v.as<T>());
            };
        }

        return {
            [](TypedScopePtr scope) {
                T value = [=]() -> T {
                    if constexpr (std::is_same_v<Scope, void>)
                        return TupleUnpacker::invoke(Getter, scope);
                    else
                        return TupleUnpacker::invoke(Getter, scope.safe_cast<Scope>());
                }();

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
        };*/
        return
        {
            []() { 
				if constexpr (std::is_base_of_v<ObservableBase, T>)
                    return OffsetPtr<Unit, T> { P }.template offset<SerializableUnitBase, ObservableBase>();
                else
                    return size_t { 0 };
			},
			[](SerializableUnitBase *_unit, SerializeInStream &in) {
                Unit *unit = static_cast<Unit *>(_unit);
                if constexpr (std::is_base_of_v<SerializableUnitBase, T>)
                    serializeTable<T>().readBinary(&(unit->*P), in);
                else 
                    in.read(unit->*P, TupleUnpacker::construct<Args>(unit)...);
            }, 
			[](const SerializableUnitBase *_unit, SerializeOutStream &out) {
                const Unit *unit = static_cast<const Unit *>(_unit);
                if constexpr (std::is_base_of_v<SerializableUnitBase, T>)
                    serializeTable<T>().writeBinary(&(unit->*P), out);
				else
					out << unit->*P;
			},
			[](SerializableUnitBase* unit, SerializeInStream& in) {
                if constexpr (std::is_base_of_v<ObservableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readAction(in, TupleUnpacker::construct<Args>(unit)...);
                else 
					throw "Unsupported";
			},
            [](SerializableUnitBase *unit, BufferedInOutStream &inout) {
                if constexpr (std::is_base_of_v<ObservableBase, T>)
                    (static_cast<Unit *>(unit)->*P).readRequest(inout, TupleUnpacker::construct<Args>(unit)...);
                else
                    throw "Unsupported";
            },
			nullptr, 
			nullptr,
            [](SerializableUnitBase *unit, const std::map<size_t, SerializableUnitBase *> &map) {
			},
            [](SerializableUnitBase * unit, bool b) {
			},
            [](SerializableUnitBase * unit, bool b) {
			}
		};
    }


}
}


#define SERIALIZETABLE_BEGIN(T) \
    namespace {            \
        namespace Serialize_##T \
        {                  \
            using Ty = T;  \
            constexpr const std::pair<const char *, ::Engine::Serialize::Serializer> fields[] = {

#define SERIALIZETABLE_END(T)              \
    {                                 \
        nullptr, { nullptr, nullptr, nullptr, nullptr, nullptr } \
    }                                 \
    }                                 \
    ;                                 \
    }                                 \
    }                                 \
    DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::Serialize::SerializeTable, ::, serializeTable, SINGLE_ARG3( { Serialize_##T::fields, std::is_base_of_v<::Engine::Serialize::TopLevelSerializableUnitBase, T> }), T);

//First argument M left out to prevent bug with empty __VA_ARGS__.
//That way it contains at least one item
#define FIELD(...) \
    { STRINGIFY2(FIRST(__VA_ARGS__)), ::Engine::Serialize::field<&Ty::__VA_ARGS__>() },
//read:   #define FIELD(M, ...) { #M, ::Engine::Serialize::field<&Ty::M, __VA_ARGS__>() },

