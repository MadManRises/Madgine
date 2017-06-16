#pragma once

#include "ogreuniquecomponentcollector.h"
#include "uniquecomponent.h"

namespace Engine{

	template <class T, class Collection, class Base = Collection>
	using OgreUniqueComponent = UniqueComponent<T, OgreUniqueComponentCollector, Collection, Base>;

}


