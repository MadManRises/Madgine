#pragma once

#include "container/physical.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION_ARGS(applySerializableMap, SerializeInStream &);
    DERIVE_FUNCTION_ARGS(setDataSynced, bool);
    DERIVE_FUNCTION_ARGS(setActive, bool, bool);
    DERIVE_FUNCTION_ARGS(setParent, SerializableUnitBase *);

    MODULES_EXPORT SerializableUnitBase *convertPtr(SerializeInStream &in, UnitId id);

    template <typename T>
    struct UnitHelper {

        static void applyMap(SerializeInStream &in, T &item)
        {
            if constexpr (std::is_pointer_v<T>) {
                uintptr_t ptr = reinterpret_cast<uintptr_t>(item);
                if (ptr & 0x1) {
                    UnitId id = (ptr >> 1);
                    item = static_cast<T>(convertPtr(in, id));
                }
            } else if constexpr (has_function_applySerializableMap_v<T>) {
                item.applySerializableMap(in);
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
            } else if constexpr (is_instance_v<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::setItemDataSynced(*item, b);
            } else if constexpr (is_iterable_v<T>) {
                for (auto &t : physical(item)) {
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

        static void setItemParent(T &item, SerializableUnitBase *parent)
        {
            if constexpr (has_function_setParent_v<T>) {
                item.setParent(parent);
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
