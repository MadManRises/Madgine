#pragma once

#include "observablecontainer.h"
#include "creationhelper.h"
#include "../../generic/container_traits.h"

namespace Engine
{
	namespace Serialize
	{
		template <class C>
		class VectorImpl : public C
		{
		public:

			typedef typename C::Type Type;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;

			using C::operator=;

			void resize(size_t size)
			{
				this->mData.resize(size);
			}

			void remove(const Type& item)
			{
				for (const_iterator it = this->begin(); it != this->end();)
				{
					if (*it == item)
					{
						it = erase(it);
					}
					else
					{
						++it;
					}
				}
			}

			void push_back(const Type& item)
			{
				emplace(this->end(), item);
			}

			template <class... _Ty>
			iterator emplace_back(_Ty&&... args)
			{
				return emplace(this->end(), std::forward<_Ty>(args)...);
			}


			template <class... _Ty>
			iterator emplace_tuple_back(std::tuple<_Ty...>&& tuple)
			{
				return TupleUnpacker::call(&VectorImpl::emplace_back<_Ty...>,
					this, 
					std::forward<std::tuple<_Ty...>>(tuple));
			}

			Type& at(size_t i)
			{
				return this->mData.at(i);
			}

			const Type& at(size_t i) const
			{
				return this->mData.at(i);
			}

			Type& operator[](size_t i)
			{
				return this->mData[i];
			}

			const Type& operator[](size_t i) const
			{
				return this->mData[i];
			}
		};

		template <class C>
		class ObservableVectorImpl : public VectorImpl<C>
		{
		public:
			using VectorImpl<C>::VectorImpl;

			typedef typename VectorImpl<C>::iterator iterator;
			typedef typename VectorImpl<C>::const_iterator const_iterator;

			using VectorImpl<C>::operator=;

			template <class... _Ty>
			void emplace_back_safe(std::function<void(const iterator&)> callback, _Ty&&... args)
			{
				insert_safe(callback, this->end(), std::forward<_Ty>(args)...);
			}
		};


		template <class T, class Creator = DefaultCreator<>>
		using SerializableVector = VectorImpl<SerializableContainer<
			container_traits<std::vector, typename UnitHelper<T>::Type>, Creator>>;

		template <class T, const ContainerPolicy &Config, class Creator = DefaultCreator<>>
		using ObservableVector = ObservableVectorImpl<ObservableContainer<
			container_traits<std::vector, typename UnitHelper<T>::Type>, Creator, Config>>;
	}
}
