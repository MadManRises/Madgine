#pragma once


#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"

#include "resourceloaderbase.h"

DEFINE_UNIQUE_COMPONENT(Engine::Resources, ResourceLoaderBase, ResourceManager&, ResourceLoader, MADGINE_BASE);
