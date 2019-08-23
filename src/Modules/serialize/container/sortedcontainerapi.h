#pragma once

#include "container.h"
#include "../../keyvalue/keyvalue.h"

namespace Engine
{
	namespace Serialize
	{
    template <typename OffsetPtr, class traits>
            class SortedContainerApi : public Container<OffsetPtr, traits>
		{
		public:

			typedef Container<OffsetPtr, traits> Base;
			using Base::Base;
			using Base::operator=;

			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;

			typedef typename traits::key_type key_type;
			typedef typename traits::value_type value_type;

			/*iterator find(const key_type& key)
			{
				return kvFind(this->mData, key);
			}*/

			bool contains(const key_type& key)
			{
				return find(key) != this->end();
			}

			template <class Ty, class _ = decltype(std::declval<typename Base::NativeContainerType>().find(std::declval<Ty>()))>
			iterator find(const Ty& v)
			{
				return this->mData.find(v);
			}
		};
	}
}
