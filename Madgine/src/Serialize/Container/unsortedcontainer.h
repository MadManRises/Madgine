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
				return access(insert_where(end(), std::forward<_Ty>(args)...));
			}

			template <class... _Ty>
			void emplace_back_safe(std::function<void(const typename Container::iterator &)> callback, _Ty&&... args) {
				insert_where_safe(callback, end(), std::forward<_Ty>(args)...);
			}

			template <class... _Ty>
			T &emplace_tuple(const iterator &where, std::tuple<_Ty...>&& tuple) {
				return access(insert_tuple_where(false, where, std::forward<std::tuple<_Ty...>>(tuple)));
			}

			template <class... _Ty>
			T &emplace_tuple_back(std::tuple<_Ty...>&& tuple) {
				return emplace_tuple(end(), std::forward<std::tuple<_Ty...>>(tuple));
			}

			void push_back(const T &other) {
				insert_where(false, end(), other);
			}

			

		protected:

			iterator read_iterator(SerializeInStream &in) {
				int i;
				in >> i;
				iterator it = begin();
				std::advance(it, i);
				return it;
			}

			void write_iterator(SerializeOutStream &out, const const_iterator &it) const {
				out << (int)std::distance(begin(), it);
			}

			iterator read_item(BufferedInOutStream &in) {
				iterator it = read_iterator(in);
				it = insert_tuple_where(true, it, readCreationData(in));
				read_id(in, access(it));
				return it;
			}

			void write_item(BufferedOutStream &out, const iterator &it) const {
				write_iterator(out, it);
				write_creation(out, it);
				write_id(out, access(it));
			}


			void read_item_save(std::function<void(const iterator &)>callback, BufferedInOutStream &in) {
				iterator it = read_iterator(in);
				insert_tuple_where_safe(callback, it, readCreationData(in));
			}
		};

	}
}