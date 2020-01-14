#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"
#include "globalapibase.h"

DECLARE_UNIQUE_COMPONENT(Engine::App, GlobalAPI, GlobalAPIBase, Application &);



namespace Engine
{
	namespace App
	{
    template <typename T>
		using GlobalAPI = GlobalAPIComponent<T>;
	}
}
