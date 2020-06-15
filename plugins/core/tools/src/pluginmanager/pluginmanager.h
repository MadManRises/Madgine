#pragma once

#if ENABLE_PLUGINS

#    include "../tinyxml/tinyxml2.h"
#    include "../toolscollector.h"
#include "../toolbase.h"
#    include "Modules/threading/slot.h"

namespace Engine {
namespace Tools {

    struct PluginManager : Tool<PluginManager> {
        PluginManager(ImRoot &root);

        virtual void render() override;

        virtual bool init() override;

        std::string_view key() const override;

        void setCurrentConfig(const Filesystem::Path &path, const std::string &name);

    protected:
        void updateConfigFile();

    private:
        Plugins::PluginManager &mManager;
        Threading::Slot<&PluginManager::setCurrentConfig> mUpdateConfigSlot;
    };

}
}

RegisterType(Engine::Tools::PluginManager);

#endif