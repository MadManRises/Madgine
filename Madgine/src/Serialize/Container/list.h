#pragma once

#include "../observable.h"
#include "../serializable.h"
#include "unsortedcontainer.h"

namespace Engine {
namespace Serialize {


	template <class T>
	class ContainerWrapper<std::list<T>> : protected UnitHelper<T> {
	protected:
		typedef std::list<T> NativeContainer;
		typedef typename NativeContainer::iterator native_iterator;
		typedef typename NativeContainer::const_iterator native_const_iterator;


		static T &access(const native_iterator &it) {
			return *it;
		}

		static const T &access(const native_const_iterator &it) {
			return *it;
		}

		template <class... _Ty>
		native_iterator insert_where_impl(const native_iterator &where, _Ty&&... args) {
			return mData.emplace(where, std::forward<_Ty>(args)...);
		}

		void erase_impl(const native_iterator &it) {
			mData.erase(it);
		}

		static void write(SerializeOutStream &out, const native_const_iterator &it) {
			UnitHelper::write(out, *it);
		}

	protected:
		NativeContainer mData;

	};
	
	template <class T, class... Args>
	class SerializableList : public UnsortedContainer<T, std::list<T>, Creator<Args...>> {
	public:
		using UnsortedContainer::UnsortedContainer;
	};
		

	template <class T, class... Args>
	class ObservableList : public ObservableContainer<SerializableList<T, Args...>> {
	public:
		using ObservableContainer::ObservableContainer;
	};



}
}


