#pragma once

#if ENABLE_PLUGINS

#    include "../tinyxml/tinyxml2.h"
#    include "../toolscollector.h"
#    include "Modules/threading/slot.h"

namespace Engine {
namespace Tools {

    class PluginManager : public Tool<PluginManager> {
    public:
        PluginManager(ImRoot &root);

        virtual void render() override;

        virtual bool init() override;

        const char *key() const override;

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