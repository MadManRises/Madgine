#pragma once

namespace Engine
{
	namespace Serialize
	{
		template <class traits, class Creator>
		class UnsortedContainer : public SerializableContainer<traits, Creator>
		{
		public:
			typedef SerializableContainer<traits, Creator> Base;
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

			std::pair<iterator, bool> read_item(SerializeInStream& in)
			{
				return this->read_item_where_intern(read_iterator(in), in);
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