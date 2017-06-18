#pragma once

#include "../serializemanager.h"
#include "serialize/streams/serializestream.h"
#include "../toplevelserializableunit.h"

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

		template <class T>
		struct UnitHelperBase : public CopyTraits<T> {
			static void read_id(SerializeInStream &in, T &item) {
			}

			static void write_id(SerializeOutStream &out, const T &item) {
			}

			static void write_creation(SerializeOutStream &out, const T &item) {
			}

			static void read_state(SerializeInStream &in, T &item) {
			}

			static void write_state(SerializeOutStream &out, const T &item) {
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return true;
			}

			static void applyMap(const std::map<size_t, SerializableUnitBase*> &map, T &item) {
			}

			static void setItemTopLevel(T &item, TopLevelSerializableUnitBase *topLevel) {
			}

			static void clearItemTopLevel(T &item) {
			}

			static void postConstruct(T &item) {
			}

			static size_t setItemInitialising(T &item) {
				return -1;
			}

			static void resetItemInitialising(T &item, size_t id) {

			}

		};


		template <class T, bool b = isValueType<T>::value>
		struct UnitHelper : public UnitHelperBase<T> {

			typedef T Type;
			
			static void read_state(SerializeInStream &in, T &item) {
				in >> item;
			}

			static void write_state(SerializeOutStream &out, const T &item) {
				out << item;
			}

		};

		template <class T>
		struct UnitHelper<const T, true> : public UnitHelperBase<const T> {

			typedef const T Type;

			static void write_creation(SerializeOutStream &out, const Type &item) {
				out << item;
			}

		};

		template <>
		struct INTERFACES_EXPORT UnitHelper<ValueType, true> : public UnitHelperBase<ValueType> {

			typedef ValueType Type;

			static void read_state(SerializeInStream &in, ValueType &item) {
				in >> item;
			}

			static void write_state(SerializeOutStream &out, const ValueType &item) {
				out << item;
			}

			static void applyMap(const std::map<size_t, SerializableUnitBase*> &map, ValueType &item);

		};

		template <class T>
		struct UnitHelper<T*, false> : public UnitHelperBase<T*> {

			typedef T* Type;

			static void read_state(SerializeInStream &in, T *&item) {
				in >> item;
			}

			static void write_state(SerializeOutStream &out, T *item) {
				out << item;
			}

		};

		template <class T>
		struct UnitHelper<std::unique_ptr<T>, false> : public UnitHelperBase<std::unique_ptr<T>> {

			typedef std::unique_ptr<typename UnitHelper<T>::Type> Type;

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

			static void applyMap(const std::map<size_t, SerializableUnitBase*> &map, const Type &item) {
				UnitHelper<T>::applyMap(map, *item);
			}

			static void setItemTopLevel(Type &item, TopLevelSerializableUnitBase *topLevel) {
				UnitHelper<T>::setItemTopLevel(*item, topLevel);
			}

			static void clearItemTopLevel(Type &item) {
				UnitHelper<T>::clearItemTopLevel(*item);
			}

			static void postConstruct(Type &item) {
				UnitHelper<T>::postConstruct(*item);
			}

		};

		template <class T>
		struct UnitHelper<T, false> : public UnitHelperBase<T> {

			typedef T Type;

			static void read_state(SerializeInStream &in, T &item) {
				item.readState(in);
			}

			static void read_id(SerializeInStream &in, SerializableUnitBase &item) {
				item.readId(in);
			}

			static void read_id(SerializeInStream &in, Serializable &item) {

			}

			static void write_id(SerializeOutStream &out, const SerializableUnitBase &item) {
				item.writeId(out);
			}

			static void write_id(SerializeOutStream &out, const Serializable &item) {

			}

			static void write_creation(SerializeOutStream &out, const Type &item) {
				item.writeCreationData(out);
			}

			static void write_state(SerializeOutStream &out, const Type &item) {
				item.writeState(out);
			}

			static bool filter(SerializeOutStream &out, const SerializableUnitBase &item) {
				return out.manager().filter(&item, out.id());
			}

			static bool filter(SerializeOutStream &out, const Serializable &item) {
				return true;
			}

			static void applyMap(const std::map<size_t, SerializableUnitBase*> &map, Type &item) {
				item.applySerializableMap(map);
			}

			static void setItemTopLevel(TopLevelSerializableUnitBase &item, TopLevelSerializableUnitBase *topLevel) {
				for (SerializeManager *mgr : topLevel->getMasterManagers()) {
					mgr->addTopLevelItem(&item, false);
				}
				SerializeManager *slaveMgr = topLevel->getSlaveManager();
				if (slaveMgr) {
					slaveMgr->addTopLevelItem(&item, false);
				}
			}

			static void setItemTopLevel(SerializableUnitBase &item, TopLevelSerializableUnitBase *topLevel) {
				item.setTopLevel(topLevel);
			}

			static void setItemTopLevel(Serializable &item, TopLevelSerializableUnitBase *topLevel) {

			}

			static void clearItemTopLevel(SerializableUnitBase &item) {
				item.clearTopLevel();
			}

			static void clearItemTopLevel(Serializable &item) {

			}

			static void postConstruct(SerializableUnitBase &item) {
				item.postConstruct();
			}

			static void postConstruct(Serializable &item) {

			}

			static size_t setItemInitialising(SerializableUnitBase &item) {
				return item.setInitialising();
			}

			static size_t setItemInitialising(Serializable &item) {
				return -1;
			}

			static void resetItemInitialising(SerializableUnitBase &item, size_t id) {
				item.resetInitialising(id);
			}

			static void resetItemInitialising(Serializable &item, size_t id) {
			}

		};


		template <class U, class V>
		struct UnitHelper<std::pair<U, V>, false> : public UnitHelperBase<std::pair<U, V>> {

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

			static void applyMap(const std::map<size_t, SerializableUnitBase*> &map, Type &item) {
				UnitHelper<U>::applyMap(map, item.first);
				UnitHelper<V>::applyMap(map, item.second);
			}

			static void setItemTopLevel(Type &item, TopLevelSerializableUnitBase *topLevel) {
				UnitHelper<U>::setItemTopLevel(item.first, topLevel);
				UnitHelper<V>::setItemTopLevel(item.second, topLevel);
			}

			static void clearItemTopLevel(Type &item) {
				UnitHelper<U>::clearItemTopLevel(item.first);
				UnitHelper<V>::clearItemTopLevel(item.second);
			}

			static void postConstruct(Type &item) {
				UnitHelper<U>::postConstruct(item.first);
				UnitHelper<V>::postConstruct(item.second);
			}

		};



	}
}
