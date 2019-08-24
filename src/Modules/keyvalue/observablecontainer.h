#pragma once

#include "container_traits.h"

namespace Engine 
{

	enum ObserverEvent
	{
		INSERT_ITEM = 0x1,
		REMOVE_ITEM = 0x2,		

		BEFORE = 0x00,
		AFTER = 0x10
	};

	

	namespace {



		template <typename _traits, typename Observer>
		struct ObservableContainerImpl : _traits::container, private Observer
		{			
			struct traits : _traits
			{
				typedef ObservableContainerImpl<_traits, Observer> container;	

				typedef typename _traits::iterator iterator;
				typedef typename _traits::const_iterator const_iterator;

				template <class... _Ty>
				static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
				{
					return c.emplace(where, std::forward<_Ty>(args)...);
				}
			};

			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;

			template <class... _Ty>
			std::pair<iterator, bool> emplace(const const_iterator &where, _Ty&&... args)
			{
				std::pair<iterator, bool> it = _traits::emplace(*this, where, std::forward<_Ty>(args)...);
				if (it.second)
					Observer::operator()(it.first, INSERT_ITEM);
				return it;
			}

			iterator erase(const iterator &where)
			{
				Observer::operator()(where, BEFORE | REMOVE_ITEM);
				iterator it = _traits::container::erase(where);
				Observer::operator()(this->end(), AFTER | REMOVE_ITEM);
				return it;
			}

			iterator erase(const iterator &from, const iterator &to)
			{
				for (iterator it = from; it != to; ++it)
				{
					Observer::operator()(it, BEFORE | REMOVE_ITEM);
				}
				iterator it = _traits::container::erase(from, to);
				for (iterator it = from; it != to; ++it)
				{
					Observer::operator()(this->end(), AFTER | REMOVE_ITEM);
				}
				return it;
			}

		};


		template <template <typename...> typename C, typename Observer>
		struct PartialObservableContainer
		{
			template <typename T>
			using type = ObservableContainerImpl<container_traits<C<T>>, Observer>;
		};

	}

	/*template <template <typename...> typename C, typename Observer>
	using PartialObservableContainer = PartialObservableContainerHelper<C, Observer>::type;*/

	template <typename C, typename Observer>
	using ObservableContainer = ObservableContainerImpl<container_traits<C>, Observer>;

}