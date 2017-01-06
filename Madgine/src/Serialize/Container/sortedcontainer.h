#pragma once 

#include "container.h"

namespace Engine {
	namespace Serialize {

		template <class NativeContainer, class Creator>
		class SortedContainer : public Container<NativeContainer, Creator>  {
		public:

			using Container::Container;

			typedef typename Container::iterator iterator;
			typedef typename Container::const_iterator const_iterator;

			typedef typename Container::Type Type;

		protected:
			

			iterator read_item(SerializeInStream &in) {
				return read_item_where(end(), in);
			}

			iterator read_item_where(const iterator &where, SerializeInStream &in) {
				iterator it = insert_tuple_where(where, readCreationData(in));
				read_id(in, *it);
				read_state(in, *it);
				return it;
			}

			void write_item(SerializeOutStream &out, const const_iterator &it) const {
				write_item(out, it, *it);
			}

			void write_item(SerializeOutStream &out, const const_iterator &it, const Type &t) const {
				write_item(out, t);
			}

			void write_item(SerializeOutStream &out, const Type &t) const {
				write_creation(out, t);
				write_id(out, t);
				write_state(out, t);
			}

		};

	}
}