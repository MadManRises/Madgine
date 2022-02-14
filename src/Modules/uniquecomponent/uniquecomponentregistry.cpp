#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "uniquecomponentregistry.h"

#    include "../plugins/binaryinfo.h"

#    include "Interfaces/filesystem/api.h"

namespace Engine {

MODULES_EXPORT std::vector<UniqueComponentRegistryBase *> &
registryRegistry()
{
    static std::vector<UniqueComponentRegistryBase *> dummy;
    return dummy;
}

} // namespace Engine

#endif