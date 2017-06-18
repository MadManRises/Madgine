#pragma once


#include "sortedcontainer.h"
#include "unsortedcontainer.h"

namespace Engine {
	namespace Serialize {


		template <class NativeContainer, class Creator>
		using ContainerSelector = 
			std::conditional_t<
				BaseContainer<NativeContainer>::sorted, 
				SortedContainer<NativeContainer, Creator>, 
				UnsortedContainer<NativeContainer, Creator>
			>;

	}
}