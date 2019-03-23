#pragma once

#include "container_traits.h"

namespace Engine 
{

	enum ObserverEvent
	{
		INSERT_ITEM = 0x1,
		REMOVE_ITEM = 0x2,
		REMOVE_RANGE = 0x3,
		RESET = 0x4,

		BEFORE = 0x00,
		AFTER = 0x10
	};

	template <typename traits, typename Observer>
	struct ObservableContainerImpl : traits::type, private Observer
	{

		typedef typename traits::iterator iterator;
		typedef typename traits::const_iterator const_iterator;

		template <class... _Ty>
		std::pair<iterator, bool> emplace(const const_iterator &where, _Ty&&... args)
		{
			std::pair<iterator, bool> it = traits::emplace(*this, where, std::forward<_Ty>(args)...);
			if (it.second)
				Observer::operator()(it.first, INSERT_ITEM);
			return it;
		}

		iterator erase(const iterator &where)
		{
			Observer::operator()(where, BEFORE | REMOVE_ITEM);
			iterator it = traits::type::erase(where);
			Observer::operator()(it, AFTER | REMOVE_ITEM);
			return it;
		}

	};

	template <typename C, typename Observer>
	using ObservableContainer = typename container_traits_helper<C>::template api<ObservableContainerImpl<container_traits_helper<C>, Observer>>;

}