#pragma once

#include "container.h"
#include "sortedcontainerapi.h"


namespace Engine
{
	namespace Serialize
	{
		template <class traits, class Creator>
		using SerializableContainer = std::conditional_t<
			traits::sorted, SortedContainerApi<traits, Creator>, Container<traits, Creator>>;
	}
}
