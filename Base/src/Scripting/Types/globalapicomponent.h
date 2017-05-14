#pragma once

#include "uniquecomponent.h"
#include "GlobalAPIComponentBase.h"
#include "Scripting\Types\scope.h"

namespace Engine {
	namespace Scripting {

	
		template <class T>
		using GlobalAPIComponent = Scope<T, BaseUniqueComponent<T, GlobalAPIComponentBase>>;


	}
}