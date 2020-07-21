#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "uniquecomponentcollectormanager.h"

#    include "uniquecomponentregistry.h"

#    include "../plugins/pluginmanager.h"

#    include "../plugins/plugin.h"

#include "../generic/container/compoundatomicoperation.h"

namespace Engine {

UniqueComponentCollectorManager::UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr)
    : mMgr(pluginMgr)
{
    mMgr.addListener(this);
}

UniqueComponentCollectorManager::~UniqueComponentCollectorManager()
{
    mMgr.removeListener(this);
}

void UniqueComponentCollectorManager::onPluginLoad(const Plugins::Plugin *p)
{

    const Plugins::BinaryInfo *info = p->info();

    CompoundAtomicOperation op;
    for (auto &[name, reg] : registryRegistry()) {
        reg->onPluginLoad(info, op);
    }
}

bool UniqueComponentCollectorManager::aboutToUnloadPlugin(const Plugins::Plugin *p)
{
    const Plugins::BinaryInfo *info = p->info();

    CompoundAtomicOperation op;
    for (auto &[name, reg] : registryRegistry()) {
        reg->onPluginUnload(info, op);
    }
    return true;
}

}

#endif