#pragma once


#include "sortedcontainer.h"
#include "unsortedcontainer.h"

namespace Engine {
	namespace Serialize {


		template <class traits, class Creator>
		using ContainerSelector = 
			std::conditional_t<
				traits::sorted, 
				SortedContainer<traits, Creator>, 
				UnsortedContainer<traits, Creator>
			>;

	}
}