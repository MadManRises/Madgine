
#include "serialize/container/unithelper.h"

namespace Engine {
	namespace Serialize {

		struct SerializableUnitHeapCreator {
			template <class T, class... _Ty>
			static T *create(_Ty&&... args) {
				T *t = new T(std::forward<_Ty>(args)...);
				UnitHelper<T>::postConstruct(*t);
				return t;
			}
		};

	}
}