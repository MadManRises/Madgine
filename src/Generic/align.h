#pragma once

namespace Engine {

	constexpr size_t alignTo(size_t size, size_t alignment) {
         return size + alignment - (((size - 1) % alignment) + 1);
	}

}