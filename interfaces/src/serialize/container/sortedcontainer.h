#pragma once 

#include "container.h"


namespace Engine {
	namespace Serialize {

		template <class NativeContainer, class Creator>
		class SortedContainer : public Container<NativeContainer, Creator>  {
		public:

			typedef Container<NativeContainer, Creator> Base;
			using Base::Base;		
			using Base::operator=;

			typedef typename Base::iterator iterator;
			typedef typename Base::const_iterator const_iterator;

			typedef typename Base::KeyType KeyType;
			typedef typename Base::Type Type;

		protected:

			iterator read_iterator(SerializeInStream &in) {
				KeyType key;
				in >> key;
				return Base::find(key);
			}

			void write_iterator(SerializeOutStream &out, const const_iterator &it) const {
				out << Base::key(it);
			}
			

			iterator read_item(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
				return this->read_item_where(this->end(), topLevel, in);
			}


			void write_item(SerializeOutStream &out, const const_iterator &it) const {
				write_item(out, it, *it);
			}

			void write_item(SerializeOutStream &out, const const_iterator &it, const Type &t) const {
				write_item(out, t);
			}

			using Base::write_item;

		};

	}
}