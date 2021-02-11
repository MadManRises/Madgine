#pragma once

#if ENABLE_PLUGINS

#include "Generic/future.h"

namespace Engine {
namespace Plugins {

    struct PluginListener {
        virtual Future<void> aboutToUnloadPlugin(const Plugin *p) { return {}; }
        virtual Future<void> aboutToLoadPlugin(const Plugin *p) { return {}; }
        virtual void onPluginUnload(const Plugin *p) { }
        virtual void onPluginLoad(const Plugin *p) { }

        virtual int priority() const { return 50; }
    };

}
}

#endif