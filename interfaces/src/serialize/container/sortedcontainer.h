#pragma once 

#include "serializablecontainer.h"
#include "generic/keyvalue.h"

namespace Engine {
	namespace Serialize {

		template <class traits, class Creator>
		class SortedContainer : public SerializableContainer<traits, Creator>  {
		public:

			typedef SerializableContainer<traits, Creator> Base;
			using Base::Base;		
			using Base::operator=;

			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;

			typedef typename traits::key_type key_type;
			typedef typename traits::type type;

		protected:

			iterator read_iterator(SerializeInStream &in) {
				key_type key;
				in >> key;
				return kvFind(this->mData, key);
			}

			void write_iterator(SerializeOutStream &out, const const_iterator &it) const {
				out << kvKey(*it);
			}
			

			iterator read_item(SerializeInStream &in) {
				return this->read_item_where(this->end(), in);
			}


			void write_item(SerializeOutStream &out, const const_iterator &it) const {
				write_item(out, it, *it);
			}

			void write_item(SerializeOutStream &out, const const_iterator &it, const type &t) const {
				write_item(out, t);
			}

			using Base::write_item;



		};

	}
}