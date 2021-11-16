#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "uniquecomponentcollectormanager.h"

#    include "uniquecomponentregistry.h"

#    include "../plugins/pluginmanager.h"

#    include "../plugins/plugin.h"

#include "../plugins/pluginsection.h"

#include "Generic/container/compoundatomicoperation.h"

namespace Engine {

UniqueComponentCollectorManager::UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr)
    : mMgr(pluginMgr)
{
    mMgr.exportSignal().connect(&exportStaticComponentHeader);

    CompoundAtomicOperation op;
    for (const auto& [name, section] : pluginMgr) {
        for (const auto& [name, plugin] : section) {
            if (plugin.isLoaded()) {
                for (UniqueComponentRegistryBase *reg : registryRegistry()) {
                    reg->onPluginLoad(plugin.info(), op);
                }
            }
        }
    }
}

}

#endif