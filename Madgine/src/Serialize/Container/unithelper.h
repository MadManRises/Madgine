#pragma once

#include "../serializemanager.h"
#include "valuetype.h"
#include "Serialize\Streams\serializestream.h"

namespace Engine {
	namespace Serialize {

		template <class T>
		class UnitCreator {
		public:
			template <class... _Ty>
			static T createTemp(_Ty&&... args) {
				return{ std::forward<_Ty>(args)... };
			}
		};

		template <class U, class V>
		class UnitCreator<std::pair<U, V>> {
		public:
			template <class... _Ty>
			static std::pair<U, V> createTemp(const U &first, _Ty&&... args) {
				return{ std::piecewise_construct, std::forward_as_tuple(first), std::forward_as_tuple(std::forward<_Ty>(args)...) };
			}
		};

		template <class T, class To, class _ = std::enable_if_t<std::is_enum<T>::value>>
		To &enum_cast(T &t) {
			return (To&)t;
		}

		template <class T, class To, class _ = std::enable_if_t<!std::is_enum<T>::value>>
		T &enum_cast(T &t) {
			return t;
		}

		template <class T, bool b = ValueType::isValueType_v<T>>
		struct UnitHelper{

			typedef T Type;



			static void read_id(SerializeInStream &in, T &item) {
			}

			static void write_id(SerializeOutStream &out, const T &item) {
			}

			static void write_creation(SerializeOutStream &out, const T &item) {
			}

			static void read_state(SerializeInStream &in, T &item) {
				in >> enum_cast<T, int>(item);
			}

			static void write_state(SerializeOutStream &out, const T &item) {
				out << enum_cast<const T, const int>(item);
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T &item) {
			}

			static void setTopLevel(T &item, TopLevelSerializableUnit *topLevel) {
			}



		};

		template <class T>
		struct UnitHelper<const T, true>{

			typedef const T Type;

			static void read_id(SerializeInStream &in, Type &item) {
			}

			static void write_id(SerializeOutStream &out, const Type &item) {
			}

			static void write_creation(SerializeOutStream &out, const Type &item) {
				out << item;
			}

			static void read_state(SerializeInStream &in, Type &item) {
			}

			static void write_state(SerializeOutStream &out, const Type &item) {
				
			}

			static bool filter(SerializeOutStream &out, const Type &item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, Type &item) {
			}

			static void setTopLevel(Type &item, TopLevelSerializableUnit *topLevel) {
			}

		};

		template <class T>
		struct UnitHelper<T*, false>{

			typedef T* Type;

			static void read_state(SerializeInStream &in, T *&item) {
				SerializableUnit *unit = in.manager().readPtr(in);
				T *temp = dynamic_cast<T*>(unit);
				if (!temp)
					throw 0;
				item = temp;
			}

			static void read_id(SerializeInStream &in, T *item) {
			}

			static void write_id(SerializeOutStream &out, T *item) {
			}

			static void write_creation(SerializeOutStream &out, T *item) {
			}

			static void write_state(SerializeOutStream &out, T *item) {
				out.manager().writePtr(out, item);
			}

			static bool filter(SerializeOutStream &out, T *item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T *&item) {
			}

			static void setTopLevel(T* &item, TopLevelSerializableUnit *topLevel) {
			}

		};

		template <>
		struct UnitHelper<ValueType, true>{

			typedef ValueType Type;

			static void read_state(SerializeInStream &in, ValueType &item) {
				in >> item;
			}

			static void read_id(SerializeInStream &in, ValueType &item) {
			}

			static void write_id(SerializeOutStream &out, const ValueType &item) {
			}

			static void write_creation(SerializeOutStream &out, const ValueType &item) {
			}

			static void write_state(SerializeOutStream &out, const ValueType &item) {
				out << item;
			}

			static bool filter(SerializeOutStream &out, const ValueType &item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, ValueType &item) {
				item.applySerializableMap(map);
			}

			static void setTopLevel(ValueType &item, TopLevelSerializableUnit *topLevel) {
			}

		};

		template <class T>
		struct UnitHelper<Ogre::unique_ptr<T>, false>{

			typedef Ogre::unique_ptr<typename UnitHelper<T>::Type> Type;

			static void read_state(SerializeInStream &in, Type &item) {
				UnitHelper<T>::read_state(in, *item);
			}

			static void read_id(SerializeInStream &in, Type &item) {
				UnitHelper<T>::read_id(in, *item);
			}

			static void write_id(SerializeOutStream &out, const Type &item) {
				UnitHelper<T>::write_id(out, *item);
			}

			static void write_creation(SerializeOutStream &out, const Type &item) {
				UnitHelper<T>::write_creation(out, *item);
			}

			static void write_state(SerializeOutStream &out, const Type &item) {
				UnitHelper<T>::write_state(out, *item);
			}

			static bool filter(SerializeOutStream &out, const Type &item) {
				return UnitHelper<T>::filter(out, *item);
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, const Type &item) {
				UnitHelper<T>::applyMap(map, *item);
			}

			static void setTopLevel(Type &item, TopLevelSerializableUnit *topLevel) {
				UnitHelper<T>::setTopLevel(*item, topLevel);
			}

		};

		template <class T>
		struct UnitHelper<T, false> {

			typedef T Type;

			static void read_state(SerializeInStream &in, T &item) {
				item.readState(in);
			}

			static void read_id(SerializeInStream &in, T &item) {
				item.readId(in);
			}

			static void write_id(SerializeOutStream &out, const T &item) {
				item.writeId(out);
			}

			static void write_creation(SerializeOutStream &out, const T &item) {
				item.writeCreationData(out);
			}

			static void write_state(SerializeOutStream &out, const T &item) {
				item.writeState(out);
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return out.manager().filter(&item, out.id());
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T &item) {
				item.applySerializableMap(map);
			}

			static void setTopLevel(T &item, TopLevelSerializableUnit *topLevel) {
				item.setTopLevel(topLevel);
			}

		};


		template <class U, class V>
		struct UnitHelper<std::pair<U, V>, false>{

			typedef std::pair<typename UnitHelper<U>::Type, typename UnitHelper<V>::Type> Type;

			static void read_state(SerializeInStream &in, Type &item) {
				UnitHelper<U>::read_state(in, item.first);
				UnitHelper<V>::read_state(in, item.second);
			}

			static void read_id(SerializeInStream &in, Type &item) {
				UnitHelper<U>::read_id(in, item.first);
				UnitHelper<V>::read_id(in, item.second);
			}

			static void write_id(SerializeOutStream &out, const Type &item) {
				UnitHelper<U>::write_id(out, item.first);
				UnitHelper<V>::write_id(out, item.second);
			}

			static void write_creation(SerializeOutStream &out, const Type &item) {
				UnitHelper<U>::write_creation(out, item.first);
				UnitHelper<V>::write_creation(out, item.second);
			}

			static void write_state(SerializeOutStream &out, const Type &item) {
				UnitHelper<U>::write_state(out, item.first);
				UnitHelper<V>::write_state(out, item.second);
			}

			static bool filter(SerializeOutStream &out, const Type &item) {
				return UnitHelper<U>::filter(out, item.first) &&
						UnitHelper<V>::filter(out, item.second);
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, Type &item) {
				UnitHelper<U>::applyMap(map, item.first);
				UnitHelper<V>::applyMap(map, item.second);
			}

			static void setTopLevel(Type &item, TopLevelSerializableUnit *topLevel) {
				UnitHelper<U>::setTopLevel(item.first, topLevel);
				UnitHelper<V>::setTopLevel(item.second, topLevel);
			}


		};



	}
}