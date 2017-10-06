#pragma once

#include "../serializable.h"
#include "container.h"
#include "sortedcontainerapi.h"

#include "signalslot/signal.h"

namespace Engine {
	namespace Serialize {


		template <class traits, class Creator>
		using SerializableContainer = std::conditional_t<traits::sorted, SortedContainerApi<traits, Creator>, Container<traits, Creator>>;


	}
}
