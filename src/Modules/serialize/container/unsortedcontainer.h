#pragma once

namespace Engine
{
	namespace Serialize
	{
    template <typename OffsetPtr, class traits>
            class UnsortedContainer : public SerializableContainer<OffsetPtr, traits>
		{
		public:
        typedef SerializableContainer<OffsetPtr, traits> Base;
			using Base::Base;
			using Base::operator=;

			typedef typename Base::iterator iterator;
			typedef typename Base::const_iterator const_iterator;

			typedef typename Base::Type Type;

		protected:


			iterator read_iterator(SerializeInStream& in)
			{
				int i;
				in >> i;
				return std::next(this->begin(), i);
			}

			void write_iterator(SerializeOutStream& out, const const_iterator& it) const
			{
				out << static_cast<int>(std::distance(this->begin(), it));
			}

			template <typename Creator>
			std::pair<iterator, bool> read_item(SerializeInStream& in, Creator &&creator)
			{
				return this->read_item_where_intern(read_iterator(in), in, std::forward<Creator>(creator));
			}

			void write_item(SerializeOutStream& out, const const_iterator& it) const
			{
				this->write_item(out, it, *it);
			}

			void write_item(SerializeOutStream& out, const const_iterator& it, const Type& t) const
			{
				write_iterator(out, it);
				this->write_item(out, t);
			}

			using Base::write_item;
		};
	}
}
