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
				return access(insert(std::forward<_Ty>(args)...));
			}

			template <class... _Ty>
			T &emplace_tuple(std::tuple<_Ty...>&& tuple) {
				return access(insert_tuple(std::forward<std::tuple<_Ty...>>(tuple)));
			}

		protected:
			template <class... _Ty>
			iterator insert_tuple(bool authorized, std::tuple<_Ty...>&& tuple) {
				return insert_tuple_where(std::forward<bool>(authorized), begin(), std::forward<std::tuple<_Ty...>>(tuple));
			}

			template <class... _Ty>
			void insert_tuple_safe(std::function<void(const iterator &)>callback, std::tuple<_Ty...>&& tuple) {
				return insert_tuple_where_safe(callback, begin(), std::forward<std::tuple<_Ty...>>(tuple));
			}

			template <class... _Ty>
			iterator insert(_Ty&&... args) {
				return insert_where(false, begin(), std::forward<_Ty>(args)...);				
			}

			iterator read_item(BufferedInOutStream &in) {
				iterator it = insert_tuple(true, readCreationData(in));		
				read_id(in, access(it));
				return it;
			}

			void read_item_save(std::function<void(const iterator &)>callback, BufferedInOutStream &in) {
				insert_tuple_safe(callback, readCreationData(in));
			}

			void write_item(BufferedOutStream &out, const iterator &it) const {
				write_creation(out, it);
				write_id(out, access(it));
			}

			void write_iterator(SerializeOutStream &out, const const_iterator &it) const {
			}

		};

	}
}