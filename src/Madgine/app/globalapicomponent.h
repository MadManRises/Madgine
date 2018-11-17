#pragma once

#include "../uniquecomponent/uniquecomponent.h"
#include "Interfaces/scripting/types/scope.h"
#include "globalapicomponentcollector.h"
#include "globalapicomponentbase.h"

namespace Engine
{

	namespace App
	{
		template <class T>
		using GlobalAPIComponent = Scripting::Scope<T, UniqueComponent<T, GlobalAPICollector>>;
	}	

}
