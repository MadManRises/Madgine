#pragma once

#include "../observable.h"
#include "../serializable.h"
#include "sortedcontainer.h"

namespace Engine {
	namespace Serialize {

		template <class T>
		class ContainerWrapper<std::map<std::string, T>> : protected UnitHelper<T> {
		protected:
			typedef std::map<std::string, Type> NativeContainer;
			typedef typename NativeContainer::iterator native_iterator;
			typedef typename NativeContainer::const_iterator native_const_iterator;


			static void write_creation(SerializeOutStream &out, const native_const_iterator &it) {
				out << it->first;
				UnitHelper::write_creation(out, it->second);
			}

			template <class... _Ty>
			native_iterator insert_where_impl(const native_iterator &where, _Ty&&... args) {
				return mData.try_emplace(where, std::forward<_Ty>(args)...);
			}

			static T &access(const native_iterator &it) {
				return it->second;
			}

			static const T &access(const native_const_iterator &it) {
				return it->second;
			}


			NativeContainer mData;
		};


		template <class T, class... Args>
		class SerializableMap : public SortedContainer<T, std::map<std::string, T>, Creator<std::string, Args...>> {
		public:
			using SortedContainer::SortedContainer;

			virtual T &operator[](const std::string &key) {
				return mData[key];
			}

			bool contains(const std::string &key) const {
				return mData.find(key) != mData.end();
			}

			const T &at(const std::string &key) const {
				return mData.at(key);
			}

			iterator find(const std::string &key) {
				return mData.find(key);
			}

		};

		template <class T, class... Args>
		class ObservableMap : public ObservableContainer<SerializableMap<T, Args...>> {
		public:
			using ObservableContainer::ObservableContainer;

		};


	}
}


