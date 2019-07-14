#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "resourceloaderbase.h"


DECLARE_UNIQUE_COMPONENT(Engine::Resources, ResourceLoaderBase, ResourceManager &, ResourceLoader, MADGINE_BASE);
