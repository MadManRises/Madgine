#pragma once

#include "../toolscollector.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

struct ImGuiDockNode;

namespace Engine {

struct MadgineObjectState;

namespace Tools {

    struct MADGINE_TOOLS_EXPORT ImRoot {
        ImRoot();
        ~ImRoot();

        virtual Threading::Task<bool> init();
        virtual Threading::Task<void> finalize();

        const std::vector<std::unique_ptr<ToolBase>> &tools();
        ToolBase &getToolComponent(size_t index);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(::Engine::component_index<T>()));
        }

        void render();

        ImGuiDockNode *dockNode() const;

        std::stringstream mToolReadBuffer;
        ToolBase *mToolReadTool = nullptr;

        void finishToolRead();

        virtual Threading::TaskQueue *taskQueue() const = 0;

    private:

        ToolsContainer<std::vector<Placeholder<0>>> mCollector;

        ImGuiDockNode *mDockNode = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Tools::ImRoot)
