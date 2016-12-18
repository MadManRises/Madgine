#pragma once 

#include "container.h"

namespace Engine {
	namespace Serialize {

		template <class T, class NativeContainer, class Creator>
		class UnsortedContainer : public Container<T, NativeContainer, Creator> {

		public:
			using Container::Container;

			T &at(size_t i) {
				iterator it = begin();
				std::advance(it, i);
				return *it;
			}

			const T &at(size_t i) const {
				const_iterator it = begin();
				std::advance(it, i);
				return *it;
			}

			template <class... _Ty>
			T &emplace(const iterator &where, _Ty&&... args) {
				return access(intern(insert_where(where, std::forward<_Ty>(args)...)));
			}

			template <class... _Ty>
			T &emplace_back(_Ty&&... args) {
				return access(intern(insert_where(end(), std::forward<_Ty>(args)...)));
			}

			template <class... _Ty>
			T &emplace_tuple(const iterator &where, std::tuple<_Ty...>&& tuple) {
				return access(intern(insert_tuple_where(where, std::forward<std::tuple<_Ty...>>(tuple))));
			}

			template <class... _Ty>
			T &emplace_tuple_back(std::tuple<_Ty...>&& tuple) {
				return emplace_tuple(end(), std::forward<std::tuple<_Ty...>>(tuple));
			}

			typename Container::iterator read_iterator(SerializeInStream &in) {
				int i;
				in >> i;
				typename Container::iterator it = begin();
				std::advance(it, i);
				return it;
			}

			void write_iterator(SerializeOutStream &out, const typename Container::const_iterator &it) const {
				out << (int)std::distance(begin(), it);
			}

			virtual void readItem(BufferedInOutStream &in) override {
				iterator it = read_iterator(in);
				insert_tuple_where_requested(it, readCreationData(in), in);
			}

		};

	}
}