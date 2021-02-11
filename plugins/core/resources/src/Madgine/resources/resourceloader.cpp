#include "../resourceslib.h"

#include "resourceloader.h"

#include "resourcemanager.h"

namespace Engine {
namespace Resources {

    ResourceLoaderBase &getLoaderByIndex(size_t i)
    {
        return ResourceManager::getSingleton().get(i);
    }

}
}