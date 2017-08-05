#pragma once 

#include "container.h"

namespace Engine {
	namespace Serialize {

		template <class NativeContainer, class Creator>
		class UnsortedContainer : public Container<NativeContainer, Creator> {

		public:
			typedef Container<NativeContainer, Creator> Base;
			using Base::Base;
			using Base::operator=;

			typedef typename Base::iterator iterator;
			typedef typename Base::const_iterator const_iterator;

			typedef typename Base::Type Type;

		protected:


			iterator read_iterator(SerializeInStream &in) {
				int i;
				in >> i;
				iterator it = this->begin();
				std::advance(it, i);
				return it;
			}

			void write_iterator(SerializeOutStream &out, const const_iterator &it) const {
				out << (int)std::distance(this->begin(), it);
			}

			iterator read_item(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
				return this->read_item_where(read_iterator(in), topLevel, in);
			}

			void write_item(SerializeOutStream &out, const const_iterator &it) const {
				write_item(out, it, *it);
			}

			void write_item(SerializeOutStream &out, const const_iterator &it, const Type &t) const {
				write_iterator(out, it);
				write_item(out, t);
			}

			using Base::write_item;


		};

	}
}