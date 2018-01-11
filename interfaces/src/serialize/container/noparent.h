#pragma once

#include "../serializableunit.h"

namespace Engine
{
	namespace Serialize
	{

			struct noparent_deleter
			{
				void operator()(SerializableUnitBase *u){
					u->deactivate();
				}
			};

		

		template <typename T, typename... Args>
		noparent_unique_ptr<T> make_noparent_unique(Args&&... args)
		{
			T *t = new T(std::forward<Args>(args)...);
			t->postConstruct();
			t->activate();
			return noparent_unique_ptr<T>(t);
		}

		template <class T>
		class NoParentUnit : public T
		{
		public:
			template <class... Args>
			NoParentUnit(Args&&... args) :
				T(std::forward<Args>(args)...)
			{
				this->postConstruct();
				this->activate();
			}

			NoParentUnit(const NoParentUnit<T>& t) :
				T(t)
			{
				this->postConstruct();
				this->activate();
			}

			NoParentUnit(NoParentUnit<T>&& t) noexcept :
				T(std::forward<NoParentUnit<T>>(t))
			{
				this->postConstruct();
				this->activate();
			}

			~NoParentUnit()
			{
				this->deactivate();
			}
		};
	}
}
