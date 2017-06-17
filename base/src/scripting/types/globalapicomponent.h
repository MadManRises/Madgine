#pragma once

#include "uniquecomponent.h"
#include "globalapicomponentbase.h"
#include "scripting/types/scope.h"

namespace Engine {
	namespace Scripting {

	
		template <class T>
		using GlobalAPIComponent = Scope<T, BaseUniqueComponent<T, GlobalAPIComponentBase>>;


	}
}