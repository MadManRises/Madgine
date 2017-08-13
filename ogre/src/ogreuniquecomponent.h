#pragma once

#include "ogreuniquecomponentcollector.h"
#include "uniquecomponent.h"

namespace Engine{

	template <class T, class Base>
	using OgreUniqueComponent = UniqueComponent<T, OgreUniqueComponentCollector, Base>;

}


