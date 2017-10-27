#pragma once

#include "unithelper.h"

namespace Engine {
	namespace Serialize {

		template <class T>
		class NoParentUnit : public T {
		public:
			template <class... _Ty>
			NoParentUnit(_Ty&&... args) :
				T(std::forward<_Ty>(args)...)
			{
				postConstruct(nullptr);
				activate();
			}

			NoParentUnit(const NoParentUnit<T> &t) :
				T(t)
			{
				postConstruct(nullptr);
				activate();
			}
			NoParentUnit(NoParentUnit<T> &&t) :
				T(std::forward<NoParentUnit<T>>(t))
			{
				postConstruct(nullptr);
				activate();
			}

			~NoParentUnit() {
				deactivate();
			}

		};


	}
}