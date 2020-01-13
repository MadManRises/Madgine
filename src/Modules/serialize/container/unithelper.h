#pragma once

#include "../primitivetypes.h"

#include "../../generic/copy_traits.h"

#include "../../generic/nullref.h"

#include "../../generic/iterator_traits.h"

namespace Engine {
namespace Serialize {
    template <class T>
    struct UnitHelperBase : public CopyTraits<T> {

        static void write_creation(SerializeOutStream &out, const T &item)
        {
        }

        static bool filter(SerializeOutStream &out, const T &item)
        {
            return true;
        }

        static void applyMap(SerializeInStream &in, T &item)
        {
        }

        static void setParent(T &item, SerializableUnitBase *parent)
        {
        }

        static void setItemDataSynced(T &item, bool b)
        {
        }

        static void setItemActive(T &item, bool active, bool existenceChanged)
        {
        }

        static void beginExtendedItem(SerializeOutStream &out, const T &item) {}
        static void beginExtendedItem(SerializeInStream &in, const T &item) {}
    };

    template <class T, bool b = isPrimitiveType_v<std::decay_t<T>>>
    struct UnitHelper : public UnitHelperBase<T> {
    };

	MODULES_EXPORT SerializableUnitBase *convertPtr(SerializeInStream &in, size_t id);

    template <class T>
    struct UnitHelper<T *, true> : public UnitHelperBase<T *> {        

        static void applyMap(SerializeInStream &in, T *&item)
        {
            size_t id = reinterpret_cast<size_t>(item);
            if (id & 0x1) {
                id >>= 1;
                item = static_cast<T *>(convertPtr(in, id));
            }
        }
    };

    template <typename T, typename D>
    struct UnitHelper<std::unique_ptr<T, D>, false> : public UnitHelperBase<std::unique_ptr<T, D>> {

        static void write_creation(SerializeOutStream &out, const std::unique_ptr<T, D> &item)
        {
            UnitHelper<T>::write_creation(out, *item);
        }

        static bool filter(SerializeOutStream &out, const std::unique_ptr<T, D> &item)
        {
            return UnitHelper<T>::filter(out, *item);
        }

        static void applyMap(SerializeInStream &in, const std::unique_ptr<T, D> &item)
        {
            UnitHelper<T>::applyMap(in, *item);
        }

        static void setParent(std::unique_ptr<T, D> &item, SerializableUnitBase *parent)
        {
            UnitHelper<T>::setParent(*item, parent);
        }

        static void setItemDataSynced(std::unique_ptr<T, D> &item, bool b)
        {
            UnitHelper<T>::setItemDataSynced(*item, b);
        }

        static void setItemActive(std::unique_ptr<T, D> &item, bool active, bool existenceChanged)
        {
            UnitHelper<T>::setItemActive(*item, active, existenceChanged);
        }

        static void beginExtendedItem(SerializeOutStream &out, const std::unique_ptr<T, D> &item)
        {
            UnitHelper<T>::beginExtendedItem(out, *item);
        }

        static void beginExtendedItem(SerializeInStream &in, const std::unique_ptr<T, D> &item)
        {
            UnitHelper<T>::beginExtendedItem(in, nullref<T>);
        }
    };

    struct MODULES_EXPORT SerializeUnitHelper {

        static bool filter(SerializeOutStream &out, const SerializableUnitBase &item);

        static bool filter(SerializeOutStream &out, const SerializableBase &item);

        static void applyMap(SerializeInStream &in, SerializableUnitBase &item);

        static void applyMap(SerializeInStream &in, SerializableBase &item);

        static void setParent(SerializableUnitBase &item, SerializableUnitBase *parent);

        static void setParent(SerializableBase &item, SerializableUnitBase *parent);

        static void beginExtendedItem(SerializeOutStream &out, const SerializableUnitBase &item);
        static void beginExtendedItem(SerializeInStream &in, const SerializableUnitBase &item);

        static void beginExtendedItem(SerializeOutStream &out, const SerializableBase &item);
        static void beginExtendedItem(SerializeInStream &in, const SerializableBase &item);
    };

    template <class T>
    struct UnitHelper<T, false> : public CopyTraits<T>, public SerializeUnitHelper {

        static void applyMap(SerializeInStream &in, T &item)
        {
            if constexpr (std::is_convertible_v<T &, SerializableBase &> || std::is_convertible_v<T &, SerializableUnitBase &>) {
                item.applySerializableMap(in);
            } else if constexpr (is_iterable_v<T>) {
                for (auto &t : item) {
                    UnitHelper<std::remove_reference_t<decltype(t)>>::applyMap(in, t);
				}
			}
        }

        static void write_creation(SerializeOutStream &out, const T &item)
        {
            //Maybe remove the call for Serializables, implying default ctor for all Serializables.
            //Only critical with containers containing serializables.
            item.writeCreationData(out);
        }

        static void setItemDataSynced(T &item, bool b)
        {
            item.setDataSynced(b);
        }

        static void setItemActive(T &item, bool active, bool existenceChanged)
        {
            item.setActive(active, existenceChanged);
        }
    };

    template <class U, class V>
    struct UnitHelper<std::pair<U, V>, false> : public UnitHelperBase<std::pair<U, V>> {

        static void write_creation(SerializeOutStream &out, const std::pair<U, V> &item)
        {
            UnitHelper<U>::write_creation(out, item.first);
            UnitHelper<V>::write_creation(out, item.second);
        }

        static bool filter(SerializeOutStream &out, const std::pair<U, V> &item)
        {
            return UnitHelper<U>::filter(out, item.first) && UnitHelper<V>::filter(out, item.second);
        }

        static void applyMap(SerializeInStream &in, std::pair<U, V> &item)
        {
            UnitHelper<U>::applyMap(in, item.first);
            UnitHelper<V>::applyMap(in, item.second);
        }

        static void setParent(std::pair<U, V> &item, SerializableUnitBase *parent)
        {
            UnitHelper<U>::setParent(item.first, parent);
            UnitHelper<V>::setParent(item.second, parent);
        }

        static void setItemDataSynced(std::pair<U, V> &item, bool b)
        {
            UnitHelper<U>::setItemDataSynced(item.first, b);
            UnitHelper<V>::setItemDataSynced(item.second, b);
        }

        static void setItemActive(std::pair<U, V> &item, bool active, bool existenceChanged)
        {
            UnitHelper<U>::setItemActive(item.first, active, existenceChanged);
            UnitHelper<V>::setItemActive(item.second, active, existenceChanged);
        }

        static void beginExtendedItem(SerializeOutStream &out, const std::pair<U, V> &item)
        {
            UnitHelper<U>::beginExtendedItem(out, item.first);
            UnitHelper<V>::beginExtendedItem(out, item.second);
        }

        static void beginExtendedItem(SerializeInStream &in, const std::pair<U, V> &item)
        {
            UnitHelper<U>::beginExtendedItem(in, nullref<const U>);
            UnitHelper<V>::beginExtendedItem(in, nullref<const V>);
        }
    };

    template <class Tuple, size_t... Is>
    struct TupleUnitHelper : public UnitHelperBase<Tuple> {
        using unpacker = bool[];

        static void write_creation(SerializeOutStream &out, const Tuple &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::write_creation(out, std::get<Is>(item)), true)...
            };
        }

        static bool filter(SerializeOutStream &out, const Tuple &item)
        {
            unpacker pack = {
                UnitHelper<typename std::tuple_element<Is, Tuple>::type>::filter(out, std::get<Is>(item))...
            };
            for (int i = 0; i < sizeof...(Is); ++i) {
                if (!pack[i])
                    return false;
            }
            return true;
        }

        static void applyMap(SerializeInStream &in, Tuple &item)
        {
            if constexpr (!std::is_same_v<Tuple, std::tuple<>>) {
                (void)unpacker {
                    (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::applyMap(in, std::get<Is>(item)), true)...
                };
            }
        }

        static void setParent(Tuple &item, SerializableUnitBase *parent)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::setParent(std::get<Is>(item), parent), true)...
            };
        }

        static void setItemDataSynced(Tuple &item, bool b)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::setItemDataSynced(std::get<Is>(item), b), true)...
            };
        }

        static void setItemActive(Tuple &item, bool active, bool existenceChanged)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::setItemActive(std::get<Is>(item), active, existenceChanged), true)...
            };
        }

        static void beginExtendedItem(SerializeOutStream &out, const Tuple &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::beginExtendedItem(out, std::get<Is>(item)), true)...
            };
        }

        static void beginExtendedItem(SerializeInStream &in, const Tuple &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, Tuple>::type>::beginExtendedItem(in, nullref<const typename std::tuple_element<Is, Tuple>::type>), true)...
            };
        }
    };

    template <typename Tuple, size_t... Is>
    TupleUnitHelper<Tuple, Is...> tupleUnitHelperDeducer(
        std::index_sequence<Is...>);

    template <class... Ty>
    struct UnitHelper<std::tuple<Ty...>, false> : public decltype(tupleUnitHelperDeducer<std::tuple<Ty...>>(
                                                      std::make_index_sequence<sizeof...(Ty)>())) {
    };

}
}
