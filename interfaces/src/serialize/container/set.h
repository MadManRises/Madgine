#pragma once

#include "creationhelper.h"
#include "observablecontainer.h"
#include "unithelper.h"
#include "generic/keyvalue.h"
#include "generic/container_traits.h"

namespace Engine
{
	namespace Serialize
	{
		template <class C>
		class SetImpl : public C
		{
		public:

			typedef typename C::value_type value_type;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;


			template <class... _Ty>
			std::pair<iterator, bool> emplace(_Ty&&... args)
			{
				return this->C::emplace(this->end(), std::forward<_Ty>(args)...);
			}
		};

		template <class C>
		using ObservableSetImpl = SetImpl<C>;

		template <class T, class Creator = DefaultCreator<>>
		using SerializableSet = SetImpl<SerializableContainer<
			container_traits<std::set, typename UnitHelper<T>::Type>, Creator>>;

		template <class T, const ContainerPolicy &Config, class Creator = DefaultCreator<>>
		using ObservableSet = ObservableSetImpl<ObservableContainer<
			container_traits<std::set, typename UnitHelper<T>::Type>, Creator, Config>>;
	}


	template <class T>
	struct container_traits<Serialize::SerializableSet, T>
	{
		static constexpr const bool sorted = true;

		typedef Serialize::SerializableSet<T> container;
		typedef SetIterator<T> iterator;
		typedef SetConstIterator<T> const_iterator;
		typedef typename KeyType<T>::type key_type;
		typedef typename container::value_type value_type;
		typedef T type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return c.emplace(std::forward<_Ty>(args)...);
		}
	};
}
