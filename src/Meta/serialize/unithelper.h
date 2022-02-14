#pragma once

#include "container/physical.h"

#include "serializableunitptr.h"

#include "streams/streamresult.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION(applySerializableMap, FormattedSerializeStream &, bool);
    DERIVE_FUNCTION(setDataSynced, bool);
    DERIVE_FUNCTION(setActive, bool, bool);
    DERIVE_FUNCTION(setParent, SerializableUnitBase *);

    META_EXPORT StreamResult convertSyncablePtr(FormattedSerializeStream &in, UnitId id, SyncableUnitBase *&out);
    META_EXPORT StreamResult convertSerializablePtr(FormattedSerializeStream &in, uint32_t id, SerializableDataUnit *&out);

    template <typename T>
    struct UnitHelper {

        static StreamResult applyMap(FormattedSerializeStream &in, T &item, bool success)
        {
            if constexpr (Pointer<T>) {
                if (success) {
                    static_assert(std::derived_from<std::remove_pointer_t<T>, SerializableDataUnit>);
                    uint32_t ptr = reinterpret_cast<uintptr_t>(item);
                    if (ptr & 0x3) {
                        switch (static_cast<UnitIdTag>(ptr & 0x3)) {
                        case UnitIdTag::SYNCABLE:
                            if constexpr (std::derived_from<std::remove_pointer_t<T>, SyncableUnitBase>) {
                                UnitId id = (ptr >> 2);
                                SyncableUnitBase *unit;
                                STREAM_PROPAGATE_ERROR(convertSyncablePtr(in, id, unit));
                                item = static_cast<T>(unit);
                            } else {
                                throw 0;
                            }
                            break;
                        case UnitIdTag::SERIALIZABLE:
                            if constexpr (!std::derived_from<std::remove_pointer_t<T>, SyncableUnitBase>) {
                                uint32_t id = (ptr >> 2);
                                SerializableDataUnit *unit;
                                STREAM_PROPAGATE_ERROR(convertSerializablePtr(in, id, unit));
                                item = static_cast<T>(unit);
                            } else {
                                throw 0;
                            }
                            break;
                        default:
                            throw 0;
                        }
                    }
                } else {
                    item = nullptr;
                }
                return {};
            } else if constexpr (has_function_applySerializableMap_v<T>) {
                return item.applySerializableMap(in, success);
            } else if constexpr (std::derived_from<T, SerializableDataUnit>) {
                return SerializableDataPtr { &item }.applySerializableMap(in, success);
            } else if constexpr (InstanceOf<std::remove_const_t<T>, std::unique_ptr>) {
                return UnitHelper<typename T::element_type>::applyMap(in, *item, success);
            } else if constexpr (std::ranges::range<T>) {
                for (auto &t : physical(item)) {
                    STREAM_PROPAGATE_ERROR(UnitHelper<std::remove_reference_t<decltype(t)>>::applyMap(in, t, success));
                }
                return {};
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
                return TupleUnpacker::accumulate(
                    TupleUnpacker::toTuple(item), [&](auto &t, StreamResult r) {
                        STREAM_PROPAGATE_ERROR(std::move(r));
                        return UnitHelper<std::remove_reference_t<decltype(t)>>::applyMap(in, t, success);
                    },
                    StreamResult {});
            } else {
                //static_assert(isPrimitiveType_v<T>, "Invalid Type");
                return {};
            }
        }

        static void setItemDataSynced(T &item, bool b)
        {
            if constexpr (has_function_setDataSynced_v<T>) {
                item.setDataSynced(b);
            } else if constexpr (std::derived_from<T, SerializableUnitBase>) {
                SerializableUnitPtr { &item }.setDataSynced(b);
            } else if constexpr (InstanceOf<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::setItemDataSynced(*item, b);
            } else if constexpr (std::ranges::range<T>) {
                for (auto &&t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemDataSynced(t, b);
                }
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
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
            } else if constexpr (std::derived_from<T, SerializableUnitBase>) {
                SerializableUnitPtr { &item }.setActive(active, existenceChanged);
            } else if constexpr (std::derived_from<T, SerializableDataUnit>) {
                SerializableDataPtr { &item }.setActive(active, existenceChanged);
            } else if constexpr (InstanceOf<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::setItemActive(*item, active, existenceChanged);
            } else if constexpr (std::ranges::range<T>) {
                for (auto &t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemActive(t, active, existenceChanged);
                }
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
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
            } else if constexpr (std::derived_from<T, SerializableUnitBase>) {
                SerializableUnitPtr { &item }.setParent(parent);
            } else if constexpr (InstanceOf<std::remove_const_t<T>, std::unique_ptr>) {
                UnitHelper<typename T::element_type>::setItemParent(*item, parent);
            } else if constexpr (std::ranges::range<T>) {
                for (auto &t : physical(item)) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::setItemParent(t, parent);
                }
            } else if constexpr (TupleUnpacker::Tuplefyable<T>) {
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
