#pragma once

#include "container/physical.h"

#include "serializableunitptr.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION(applySerializableMap, SerializeInStream &);
    DERIVE_FUNCTION(setDataSynced, bool);
    DERIVE_FUNCTION(setActive, bool, bool);
    DERIVE_FUNCTION(setParent, SerializableUnitBase *);

    META_EXPORT SyncableUnitBase *convertSyncablePtr(SerializeInStream &in, UnitId id);
    META_EXPORT SerializableDataUnit *convertSerializablePtr(SerializeInStream &in, uint32_t id);

    template <typename T>
    struct UnitHelper {

        static void applyMap(SerializeInStream &in, T &item)
        {
            if constexpr (std::is_pointer_v<T>) {
                static_assert(std::is_base_of_v<SerializableDataUnit, std::remove_pointer_t<T>>);
                uint32_t ptr = reinterpret_cast<uintptr_t>(item);
                if (ptr & 0x3) {
                    switch (static_cast<UnitIdTag>(ptr & 0x3)) {
                    case UnitIdTag::SYNCABLE:
                        if constexpr (std::is_base_of_v<SyncableUnitBase, std::remove_pointer_t<T>>) {
                            UnitId id = (ptr >> 2);
                            item = static_cast<T>(convertSyncablePtr(in, id));
                        } else {
                            throw 0;
                        }
                        break;
                    case UnitIdTag::SERIALIZABLE:
                        if constexpr (!std::is_base_of_v<SyncableUnitBase, std::remove_pointer_t<T>>) {
                            uint32_t id = (ptr >> 2);
                            item = static_cast<T>(convertSerializablePtr(in, id));
                        } else {
                            throw 0;
                        }
                        break;
                    default:
                        throw 0;
                    }                    
                }
            } else if constexpr (has_function_applySerializableMap_v<T>) {
                item.applySerializableMap(in);
            } else if constexpr (std::is_base_of_v<SerializableDataUnit, T>){
                SerializableDataPtr { &item }.applySerializableMap(in);
            } else if constexpr (is_instance_v<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::applyMap(in, *item);
            } else if constexpr (is_iterable_v<T>) {
                for (auto &t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::applyMap(in, t);
                }
            } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::applyMap(in, t);
                });
            } else {
                //static_assert(isPrimitiveType_v<T>, "Invalid Type");
            }
        }

                static void setItemDataSynced(T &item, bool b)
        {
            if constexpr (has_function_setDataSynced_v<T>) {
                item.setDataSynced(b);
            } else if constexpr (std::is_base_of_v<SerializableUnitBase, T>) {
                SerializableUnitPtr { &item }.setDataSynced(b);
            } else if constexpr (is_instance_v<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::setItemDataSynced(*item, b);
            } else if constexpr (is_iterable_v<T>) {
                for (auto &&t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemDataSynced(t, b);
                }
            } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemDataSynced(t, b);
                });
            } else {
                //static_assert(isPrimitiveType_v<T>, "Invalid Type");
            }
        }

        static void setItemActive(T &item, bool active, bool existenceChanged)
        {
            if constexpr (has_function_setActive_v<T>) {
                item.setActive(active, existenceChanged);
            } else if constexpr (std::is_base_of_v<SerializableUnitBase, T>) {
                SerializableUnitPtr { &item }.setActive(active, existenceChanged);
            } else if constexpr (is_instance_v<std::remove_const_t<T>, std::unique_ptr>) {                
                UnitHelper<typename T::element_type>::setItemActive(*item, active, existenceChanged);
            } else if constexpr (is_iterable_v<T>) {
                for (auto &t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemActive(t, active, existenceChanged);
                }
            } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemActive(t, active, existenceChanged);
                });
            } else {
                //static_assert(isPrimitiveType_v<T>, "Invalid Type");
            }
        }

        static void setItemParent(T &item, SerializableDataUnit *parent)
        {
        }

        static void setItemParent(T &item, SerializableUnitBase *parent)
        {
            if constexpr (has_function_setParent_v<T>) {
                item.setParent(parent);
            } else if constexpr (std::is_base_of_v<SerializableUnitBase, T>) {
                SerializableUnitPtr { &item }.setParent(parent);
            } else if constexpr (is_instance_v<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::setItemParent(*item, parent);
            } else if constexpr (is_iterable_v<T>) {
                for (auto &t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemParent(t, parent);
                }
            } else if constexpr (TupleUnpacker::is_tuplefyable_v<T>) {
                TupleUnpacker::forEach(TupleUnpacker::toTuple(item), [&](auto &t) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemParent(t, parent);
                });
            } else {
                //static_assert(isPrimitiveType_v<T>, "Invalid Type");
            }
        }
    };

}
}
