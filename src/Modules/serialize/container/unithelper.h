#pragma once

#include "../streams/serializestream.h"

#include "../../generic/copy_traits.h"

//#include "../../keyvalue/valuetype.h"

namespace Engine {
namespace Serialize {
    template <class T>
    struct UnitHelperBase : public CopyTraits<T> {
        static void read_id(SerializeInStream &in, T &item)
        {
        }

        static void write_id(SerializeOutStream &out, const T &item)
        {
        }

        static void write_creation(SerializeOutStream &out, const T &item)
        {
        }

        static void read_state(SerializeInStream &in, T &item)
        {
        }

        static void write_state(SerializeOutStream &out, const T &item)
        {
        }

        static bool filter(SerializeOutStream &out, const T &item)
        {
            return true;
        }

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, T &item)
        {
        }

        static void setParent(T &item, SerializableUnitBase *parent)
        {
        }

        static void setItemDataSynced(T &item, bool b)
        {
        }

        static void setItemActive(T &item, bool active)
        {
        }
    };

    template <class T, bool b = PrimitiveTypesContain_v<std::decay_t<T>>>
    struct UnitHelper : public UnitHelperBase<T> {
        typedef T type;

        static void read_state(SerializeInStream &in, type &item)
        {
            in >> item;
        }

        static void write_state(SerializeOutStream &out, const type &item)
        {
            out << item;
        }
    };

    template <class T>
    struct UnitHelper<const T, true> : public UnitHelperBase<const T> {
        typedef const T type;

        static void write_creation(SerializeOutStream &out, const type &item)
        {
            out << item;
        }
    };

    /*template <>
    struct MODULES_EXPORT UnitHelper<ValueType, false> : public UnitHelperBase<ValueType> {
        typedef ValueType type;

        static void read_state(SerializeInStream &in, ValueType &item)
        {
            in >> item;
        }

        static void write_state(SerializeOutStream &out, const ValueType &item)
        {
            out << item;
        }

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, ValueType &item)
        {
        }
    };*/

    template <class T>
    struct UnitHelper<T *, false> : public UnitHelperBase<T *> {
        typedef T *type;

        static void read_state(SerializeInStream &in, T *&item)
        {
            in >> item;
        }

        static void write_state(SerializeOutStream &out, T *item)
        {
            out << item;
        }
    };

    template <class T>
    struct UnitHelper<std::unique_ptr<T>, false> : public UnitHelperBase<std::unique_ptr<T>> {
        typedef std::unique_ptr<typename UnitHelper<T>::type> type;

        static void read_state(SerializeInStream &in, type &item)
        {
            UnitHelper<T>::read_state(in, *item);
        }

        static void read_id(SerializeInStream &in, type &item)
        {
            UnitHelper<T>::read_id(in, *item);
        }

        static void write_id(SerializeOutStream &out, const type &item)
        {
            UnitHelper<T>::write_id(out, *item);
        }

        static void write_creation(SerializeOutStream &out, const type &item)
        {
            UnitHelper<T>::write_creation(out, *item);
        }

        static void write_state(SerializeOutStream &out, const type &item)
        {
            UnitHelper<T>::write_state(out, *item);
        }

        static bool filter(SerializeOutStream &out, const type &item)
        {
            return UnitHelper<T>::filter(out, *item);
        }

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, const type &item)
        {
            UnitHelper<T>::applyMap(map, *item);
        }

        static void setParent(type &item, SerializableUnitBase *parent)
        {
            UnitHelper<T>::setParent(*item, parent);
        }

        static void setItemDataSynced(type &item, bool b)
        {
            UnitHelper<T>::setItemDataSynced(*item, b);
        }

        static void setItemActive(type &item, bool active)
        {
            UnitHelper<T>::setItemActive(*item, active);
        }
    };

    struct MODULES_EXPORT SerializeUnitHelper {
        static void read_state(SerializeInStream &in, SerializableBase &item);

        static void read_state(SerializeInStream &in, SerializableUnitBase &unit);

        static void read_id(SerializeInStream &in, SerializableUnitBase &item);

        static void read_id(SerializeInStream &in, SerializableBase &item);

        static void write_id(SerializeOutStream &out, const SerializableUnitBase &item);

        static void write_id(SerializeOutStream &out, const SerializableBase &item);

        static void write_creation(SerializeOutStream &out, const SerializableBase &item);

        static void write_creation(SerializeOutStream &out, const SerializableUnitBase &item);

        static void write_state(SerializeOutStream &out, const SerializableBase &item);

        static void write_state(SerializeOutStream &out, const SerializableUnitBase &unit);

        static bool filter(SerializeOutStream &out, const SerializableUnitBase &item);

        static bool filter(SerializeOutStream &out, const SerializableBase &item);

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, SerializableUnitBase &item);

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, SerializableBase &item);

        static void setParent(SerializableUnitBase &item, SerializableUnitBase *parent);

        static void setParent(SerializableBase &item, SerializableUnitBase *parent);

        static void setItemDataSynced(SerializableUnitBase &item, bool b);

        static void setItemDataSynced(SerializableBase &item, bool b);

        static void setItemActive(SerializableUnitBase &item, bool active);

        static void setItemActive(SerializableBase &item, bool active);
    };

    template <class T>
    struct UnitHelper<T, false> : public CopyTraits<T>, public SerializeUnitHelper {
        typedef T type;

        static void read_state(SerializeInStream &in, type &item)
        {
            item.readState(in);
        }

        static void write_state(SerializeOutStream &out, const type &item)
        {
            item.writeState(out);
        }


        static void write_creation(SerializeOutStream &out, const type &item)
        {
			//Maybe remove the call for Serializables, implying default ctor for all Serializables.
			//Only critical with containers containing serializables.
            item.writeCreationData(out);
        }
    };

    template <class U, class V>
    struct UnitHelper<std::pair<U, V>, false> : public UnitHelperBase<std::pair<U, V>> {
        typedef std::pair<typename UnitHelper<U>::type, typename UnitHelper<V>::type> type;

        static void read_state(SerializeInStream &in, type &item)
        {
            UnitHelper<U>::read_state(in, item.first);
            UnitHelper<V>::read_state(in, item.second);
        }

        static void read_id(SerializeInStream &in, type &item)
        {
            UnitHelper<U>::read_id(in, item.first);
            UnitHelper<V>::read_id(in, item.second);
        }

        static void write_id(SerializeOutStream &out, const type &item)
        {
            UnitHelper<U>::write_id(out, item.first);
            UnitHelper<V>::write_id(out, item.second);
        }

        static void write_creation(SerializeOutStream &out, const type &item)
        {
            UnitHelper<U>::write_creation(out, item.first);
            UnitHelper<V>::write_creation(out, item.second);
        }

        static void write_state(SerializeOutStream &out, const type &item)
        {
            UnitHelper<U>::write_state(out, item.first);
            UnitHelper<V>::write_state(out, item.second);
        }

        static bool filter(SerializeOutStream &out, const type &item)
        {
            return UnitHelper<U>::filter(out, item.first) && UnitHelper<V>::filter(out, item.second);
        }

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, type &item)
        {
            UnitHelper<U>::applyMap(map, item.first);
            UnitHelper<V>::applyMap(map, item.second);
        }

        static void setParent(type &item, SerializableUnitBase *parent)
        {
            UnitHelper<U>::setParent(item.first, parent);
            UnitHelper<V>::setParent(item.second, parent);
        }

        static void setItemDataSynced(type &item, bool b)
        {
            UnitHelper<U>::setItemDataSynced(item.first, b);
            UnitHelper<V>::setItemDataSynced(item.second, b);
        }

        static void setItemActive(type &item, bool active)
        {
            UnitHelper<U>::setItemActive(item.first, active);
            UnitHelper<V>::setItemActive(item.second, active);
        }
    };

    template <class Tuple, size_t... Is>
    struct TupleUnitHelper : public UnitHelperBase<Tuple> {
        typedef Tuple type;
        using unpacker = bool[];

        static void read_state(SerializeInStream &in, type &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::read_state(in, std::get<Is>(item)), true)...
            };
            /*UnitHelper<U>::read_state(in, item.first);
				UnitHelper<V>::read_state(in, item.second);*/
        }

        static void read_id(SerializeInStream &in, type &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::read_id(in, std::get<Is>(item)), true)...
            };
        }

        static void write_id(SerializeOutStream &out, const type &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::write_id(out, std::get<Is>(item)), true)...
            };
        }

        static void write_creation(SerializeOutStream &out, const type &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::write_creation(out, std::get<Is>(item)), true)...
            };
        }

        static void write_state(SerializeOutStream &out, const type &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::write_state(out, std::get<Is>(item)), true)...
            };
        }

        static bool filter(SerializeOutStream &out, const type &item)
        {
            unpacker pack = {
                UnitHelper<typename std::tuple_element<Is, type>::type>::filter(out, std::get<Is>(item))...
            };
            for (int i = 0; i < sizeof...(Is); ++i) {
                if (!pack[i])
                    return false;
            }
            return true;
        }

        static void applyMap(const std::map<size_t, SerializableUnitBase *> &map, type &item)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::applyMap(map, std::get<Is>(item)), true)...
            };
        }

        static void setParent(type &item, SerializableUnitBase *parent)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::setParent(std::get<Is>(item), parent), true)...
            };
        }

        static void setItemDataSynced(type &item, bool b)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::setItemDataSynced(std::get<Is>(item), b), true)...
            };
        }

        static void setItemActive(type &item, bool active)
        {
            (void)unpacker {
                (UnitHelper<typename std::tuple_element<Is, type>::type>::setItemActive(std::get<Is>(item), active), true)...
            };
        }
    };

    template <class... Ty, size_t... Is>
    TupleUnitHelper<std::tuple<typename UnitHelper<Ty>::type...>, Is...> tupleUnitHelperDeducer(
        std::index_sequence<Is...>);

    template <class... Ty>
    struct UnitHelper<std::tuple<Ty...>, false> : public decltype(tupleUnitHelperDeducer<Ty...>(
                                                      std::make_index_sequence<sizeof...(Ty)>())) {
    };

	template <typename T>
		using UnitHelper_t = typename UnitHelper<T>::type;

}
}
