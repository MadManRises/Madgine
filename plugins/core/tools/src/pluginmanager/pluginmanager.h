#pragma once

#if ENABLE_PLUGINS

#    include "../toolbase.h"
#    include "../toolscollector.h"

namespace Engine {
namespace Tools {

    struct PluginManager : Tool<PluginManager> {
        SERIALIZABLEUNIT(PluginManager);

        PluginManager(ImRoot &root);

        virtual void render() override;

        virtual bool init() override;

        std::string_view key() const override;

        void setCurrentConfig(const Filesystem::Path &path, const std::string &name);

    protected:
        void updateConfigFile();

    private:
        Plugins::PluginManager &mManager;
    };

}
}

RegisterType(Engine::Tools::PluginManager);

#endif