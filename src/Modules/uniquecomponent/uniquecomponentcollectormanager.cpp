#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "uniquecomponentcollectormanager.h"

#    include "uniquecomponentregistry.h"

#    include "../plugins/pluginmanager.h"

#    include "../plugins/plugin.h"

#include "Generic/container/compoundatomicoperation.h"

namespace Engine {

UniqueComponentCollectorManager::UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr)
    : mMgr(pluginMgr)
{
    mMgr.addListener(this);
    mMgr.exportSignal().connect(&exportStaticComponentHeader);
}

UniqueComponentCollectorManager::~UniqueComponentCollectorManager()
{
    mMgr.removeListener(this);
}

void UniqueComponentCollectorManager::onPluginLoad(const Plugins::Plugin *p)
{

    const Plugins::BinaryInfo *info = p->info();

    CompoundAtomicOperation op;
    for (UniqueComponentRegistryBase *reg : registryRegistry()) {
        reg->onPluginLoad(info, op);
    }
}

Threading::TaskFuture<void> UniqueComponentCollectorManager::aboutToUnloadPlugin(const Plugins::Plugin *p)
{
    const Plugins::BinaryInfo *info = p->info();

    CompoundAtomicOperation op;
    for (UniqueComponentRegistryBase *reg : registryRegistry()) {
        reg->onPluginUnload(info, op);
    }
    return {};
}

}

#endif