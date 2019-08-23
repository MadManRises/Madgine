#pragma once

#include "creationhelper.h"
#include "observablecontainer.h"
#include "../../keyvalue/container_traits.h"

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

			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple(std::tuple<_Ty...>&& tuple)
			{
				return TupleUnpacker::invokeExpand(&SetImpl::emplace<_Ty...>,
					this, 
					std::forward<std::tuple<_Ty...>>(tuple));
			}

		};

		template <class C>
		using ObservableSetImpl = SetImpl<C>;

		template <typename PtrOffset, class T>
		using SerializableSet = SetImpl<SerializableContainer<PtrOffset,
			container_traits<std::set, typename UnitHelper<T>::Type>>>;

		template <typename PtrOffset, class T, typename Config>
		using ObservableSet = ObservableSetImpl<ObservableContainer<PtrOffset, 
			container_traits<std::set, typename UnitHelper<T>::Type>, Config>>;
	}

}
