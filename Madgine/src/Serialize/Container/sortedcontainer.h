#pragma once 

#include "container.h"

namespace Engine {
	namespace Serialize {

		template <class T, class NativeContainer, class Creator>
		class SortedContainer : public Container<T, NativeContainer, Creator> {
		public:

			using Container::Container;

			template <class... _Ty>
			T &emplace(_Ty&&... args) {
				return access(intern(insert(std::forward<_Ty>(args)...)));
			}

			template <class... _Ty>
			T &emplace_tuple(std::tuple<_Ty...>&& tuple) {
				return access(intern(insert_tuple(std::forward<std::tuple<_Ty...>>(tuple))));
			}

		protected:
			template <class... _Ty>
			iterator insert_tuple(std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<>::call(this, &SortedContainer::insert<_Ty...>, std::forward<std::tuple<_Ty...>>(tuple));
			}

			template <class... _Ty>
			iterator insert(_Ty&&... args) {
				iterator it(insert_impl(std::forward<_Ty>(args)...), *this);
				onInsert(it);
				return it;
			}

			virtual void readItem(BufferedInOutStream &in) override {
				insert_tuple(readCreationData(in));
			}

			/*typename Container::iterator read_iterator(SerializeInStream &) {
				return end();
			}*/
			void write_iterator(SerializeOutStream &, const typename Container::const_iterator &) const {
			}

		};

	}
}