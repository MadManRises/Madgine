#pragma once

#if ENABLE_PLUGINS

namespace Engine {
namespace UniqueComponent {

    struct MODULES_EXPORT CollectorManager {
        CollectorManager(Plugins::PluginManager &pluginMgr);
    };

}
}

#endif