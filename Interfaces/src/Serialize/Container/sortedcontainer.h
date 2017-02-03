#pragma once 

#include "container.h"

namespace Engine {
	namespace Serialize {

		template <class NativeContainer, class Creator>
		class SortedContainer : public Container<NativeContainer, Creator>  {
		public:

			using Container<NativeContainer, Creator>::Container;

			typedef typename Container<NativeContainer, Creator>::iterator iterator;
			typedef typename Container<NativeContainer, Creator>::const_iterator const_iterator;

			typedef typename Container<NativeContainer, Creator>::Type Type;

		protected:
			

			iterator read_item(SerializeInStream &in) {
				return read_item_where(this->end(), in);
			}

			iterator read_item_where(const iterator &where, SerializeInStream &in) {
				iterator it = this->insert_tuple_where(where, this->readCreationData(in));
				this->read_id(in, *it);
				this->read_state(in, *it);
				return it;
			}

			void write_item(SerializeOutStream &out, const const_iterator &it) const {
				write_item(out, it, *it);
			}

			void write_item(SerializeOutStream &out, const const_iterator &it, const Type &t) const {
				write_item(out, t);
			}

			void write_item(SerializeOutStream &out, const Type &t) const {
				this->write_creation(out, t);
				this->write_id(out, t);
				this->write_state(out, t);
			}

		};

	}
}