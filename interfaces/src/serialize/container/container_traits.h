
#pragma once

namespace Engine {
	namespace Serialize {

		template <template <class...> class C, class T>
		struct container_traits {
			static constexpr const bool sorted = false;

			typedef C<T> container;
			typedef typename container::iterator iterator;
			typedef typename container::const_iterator const_iterator;
			typedef typename container::value_type value_type;
			typedef void key_type;
			typedef T type;

			template <class... _Ty>
			static iterator insert(container &c, const const_iterator &where, _Ty&&... args) {
				return c.emplace(where, std::forward<_Ty>(args)...);
			}

			static void write_iterator(SerializeOutStream &out, const const_iterator &it) {

			}

		};

	}
}
