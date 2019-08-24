#pragma once

#include "syncablecontainer.h"
#include "creationhelper.h"
#include "../../keyvalue/container_traits.h"

namespace Engine
{
	namespace Serialize
	{
		template <class C>
		class ListImpl : public C
		{
		public:

			typedef typename C::type type;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;

			void remove(const type& item)
			{
				for (iterator it = this->begin(); it != this->end();)
				{
					if (*it == item)
					{
						it = this->erase(it);
					}
					else
					{
						++it;
					}
				}
			}

			void push_back(const type& item)
			{
				this->emplace(this->end(), item);
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace_back(_Ty&&... args)
			{
				return this->emplace(this->end(), std::forward<_Ty>(args)...);
			}

			const type& back() const
			{
				return this->mData.back();
			}

			type& back()
			{
				return this->mData.back();
			}
		};

		template <class C>
                class SyncableListImpl : public ListImpl<C>
		{
		public:
			using ListImpl<C>::ListImpl;

			typedef typename ListImpl<C>::iterator iterator;
			typedef typename ListImpl<C>::const_iterator const_iterator;

			template <class T, class... _Ty>
			void emplace_back_init(T&& init, _Ty&&... args)
			{
				this->emplace_init(std::forward<T>(init), this->end(), std::forward<_Ty>(args)...);
			}

		};

		template <typename OffsetPtr, class T>
		using SerializableList = ListImpl<SerializableContainer<OffsetPtr, 
			container_traits<std::list<typename UnitHelper<T>::type>>>>;

		template <typename PtrOffset, class T, typename Config>
		using ObservableList = SyncableListImpl<SyncableContainer<PtrOffset,
			container_traits<std::list<typename UnitHelper<T>::type>>, Config>>;
	}
}
