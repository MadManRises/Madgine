#pragma once

#include "creationhelper.h"
#include "observablecontainer.h"
#include "../../generic/keyvalue.h"
#include "../../generic/container_traits.h"

namespace Engine
{
	namespace Serialize
	{
		template <class C>
		class UnorderedSetImpl : public C
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

			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple(std::tuple<_Ty...>&& tuple)
			{
				return TupleUnpacker<>::call(this, &SetImpl::emplace<_Ty...>, std::forward<std::tuple<_Ty...>>(tuple));
			}

		};

		template <class C>
		using ObservableUnorderedSetImpl = UnorderedSetImpl<C>;

		template <class T, class Creator = DefaultCreator<>>
		using SerializableUnorderedSet = UnorderedSetImpl<SerializableContainer<
			container_traits<std::unordered_set, typename UnitHelper<T>::Type>, Creator>>;

		template <class T, const ContainerPolicy &Config, class Creator = DefaultCreator<>>
		using ObservableUnorderedSet = ObservableUnorderedSetImpl<ObservableContainer<
			container_traits<std::unordered_set, typename UnitHelper<T>::Type>, Creator, Config>>;
	}


	template <class T>
	struct container_traits<Serialize::SerializableUnorderedSet, T>
	{
		static constexpr const bool sorted = true;

		typedef Serialize::SerializableUnorderedSet<T> container;
		typedef UnorderedSetIterator<T> iterator;
		typedef UnorderedSetConstIterator<T> const_iterator;
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
