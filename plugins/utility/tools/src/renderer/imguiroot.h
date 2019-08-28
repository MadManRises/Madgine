#pragma once

#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Madgine/threading/framelistener.h"

#include "../toolscollector.h"

#include "Modules/plugins/pluginlistener.h"

namespace Engine {
namespace Tools {

    class MADGINE_TOOLS_EXPORT ImGuiRoot : public GUI::TopLevelWindowComponent<ImGuiRoot>, public Threading::FrameListener
#if ENABLE_PLUGINS
        ,
                                           public Plugins::PluginListener
#endif
    {
    public:
        ImGuiRoot(GUI::TopLevelWindow &window);
        ~ImGuiRoot();

        bool init() override;
        void finalize() override;

        bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

#if ENABLE_PLUGINS
        bool aboutToUnloadPlugin(const Plugins::Plugin *p) override;
        void onPluginLoad(const Plugins::Plugin *p) override;
#endif

        const ToolsContainer<std::vector> &tools();
        ToolBase &getToolComponent(size_t index, bool = true);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(::Engine::component_index<T>()));
        }

		const ImGuiManager &manager() const;

    private:
        void createManager();
        void destroyManager();

    private:
        std::unique_ptr<ImGuiManager> mManager;

        ToolsContainer<std::vector> mCollector;
    };

}
}
