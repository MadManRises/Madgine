#pragma once

#include "serializablecontainer.h"
#include "../../keyvalue/keyvalue.h"

namespace Engine
{
	namespace Serialize
	{
    template <typename OffsetPtr, class traits>
            class SortedContainer : public SerializableContainer<OffsetPtr, traits>
		{
		public:

			typedef SerializableContainer<OffsetPtr, traits> Base;
			using Base::Base;
			using Base::operator=;

			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;

			typedef typename traits::key_type key_type;
			typedef typename traits::type type;

		protected:

			iterator read_iterator(SerializeInStream& in)
			{
				key_type key;
				in >> key;
				return kvFind(this->mData, key);
			}

			void write_iterator(SerializeOutStream& out, const const_iterator& it) const
			{
				out << kvKey(*it);
			}


			std::pair<iterator, bool> read_item(SerializeInStream& in)
			{
				return this->read_item_where_intern(this->end(), in);
			}


			void write_item(SerializeOutStream& out, const const_iterator& it) const
			{
				this->write_item(out, it, *it);
			}

			void write_item(SerializeOutStream& out, const const_iterator& it, const type& t) const
			{
				this->write_item(out, t);
			}

			using Base::write_item;
		};
	}
}
