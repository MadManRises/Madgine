#pragma once

#include "creationhelper.h"
#include "syncablecontainer.h"
#include "../../keyvalue/container_traits.h"
#include "sortedcontainerapi.h"

namespace Engine
{
	namespace Serialize
	{
		template <typename C>
		struct SetImpl : SortedContainerApi<C>
		{
			using Base = SortedContainerApi<C>;

			typedef typename Base::value_type value_type;

			typedef typename Base::iterator iterator;
			typedef typename Base::const_iterator const_iterator;

			using Base::Base;


			template <class... _Ty>
			std::pair<iterator, bool> emplace(_Ty&&... args)
			{
				return this->C::emplace(this->end(), std::forward<_Ty>(args)...);
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple(std::tuple<_Ty...>&& tuple)
			{
				return TupleUnpacker::invokeExpand(&SetImpl::emplace<_Ty...>,
					this, 
					std::forward<std::tuple<_Ty...>>(tuple));
			}

		};

		template <class C>
		using SyncableSetImpl = SetImpl<C>;

		template <typename PtrOffset, class T>
		using SerializableSet = SetImpl<SerializableContainer<PtrOffset,
			container_traits<std::set<typename UnitHelper<T>::type>>>>;

		template <typename PtrOffset, class T, typename Config>
		using SyncableSet = SyncableSetImpl<SyncableContainer<PtrOffset, 
			container_traits<std::set<typename UnitHelper<T>::type>>, Config>>;
	}

}
