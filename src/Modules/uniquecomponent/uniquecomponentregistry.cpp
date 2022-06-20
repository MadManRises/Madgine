#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "uniquecomponentregistry.h"

#    include "../plugins/binaryinfo.h"

#    include "Interfaces/filesystem/api.h"

namespace Engine {
namespace UniqueComponent {

    MODULES_EXPORT std::vector<RegistryBase *> &
    registryRegistry()
    {
        static std::vector<RegistryBase *> dummy;
        return dummy;
    }

}
} // namespace Engine

#endif