#pragma once

#if ENABLE_PLUGINS

#    include "Modules/threading/taskfuture.h"

namespace Engine {
namespace Plugins {

    struct PluginListener {
        virtual Threading::TaskFuture<void> aboutToUnloadPlugin(const Plugin *p) { return {}; }
        virtual Threading::TaskFuture<void> aboutToLoadPlugin(const Plugin *p) { return {}; }
        virtual void onPluginUnload(const Plugin *p) { }
        virtual void onPluginLoad(const Plugin *p) { }

        virtual int priority() const { return 50; }
    };

}
}

#endif