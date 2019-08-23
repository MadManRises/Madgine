#pragma once

#include "container.h"
#include "sortedcontainerapi.h"


namespace Engine
{
	namespace Serialize
	{
    template <typename OffsetPtr, class traits>
		using SerializableContainer = std::conditional_t<
                traits::sorted, SortedContainerApi<OffsetPtr, traits>, Container<OffsetPtr, traits>>;



	template <template <typename...> typename C, typename OffsetPtr>
    struct PartialSerializableContainer {
        template <typename... Args>
        using type = C<OffsetPtr, Args...>;
    };

	}
}
