#pragma once

#include "../generic/proxy.h"

namespace Engine {
namespace Threading {

	struct GlobalStorage {
    
		template <typename T>
		using container_type = Proxy<T, false>;
	
	};

}
}