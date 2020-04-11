#include "../moduleslib.h"

#include "resourceloader.h"

#include "resourcemanager.h"

namespace Engine {
namespace Resources {

    ResourceLoaderBase &Engine::Resources::getLoaderByIndex(size_t i)
    {
        return ResourceManager::getSingleton().get(i);
    }

}
}