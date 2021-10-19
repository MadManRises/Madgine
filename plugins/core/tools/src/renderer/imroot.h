#pragma once

#include "../toolscollector.h"

#include "Generic/container/observablecontainer.h"

#include "madgineobject/madgineobjectobserver.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

struct ImGuiDockNode;

namespace Engine {

    struct MadgineObjectState;

namespace Tools {

    struct MADGINE_TOOLS_EXPORT ImRoot {
        ImRoot(MadgineObjectState *state);
        ~ImRoot();

        virtual bool init();
        virtual void finalize();

        const std::vector<std::unique_ptr<ToolBase>> &tools();
        ToolBase &getToolComponent(size_t index, bool = true);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(::Engine::component_index<T>()));
        }

        bool frame();

        ImGuiDockNode *dockNode() const;

        bool isInitialized() const;
        
        std::stringstream mToolReadBuffer;
        ToolBase *mToolReadTool = nullptr;

        void finishToolRead();

    protected:
        void checkInitState();

    private:
        MadgineObjectState *mState;

        ToolsContainer<ObservableContainer<std::vector<Placeholder<0>>, MadgineObjectObserver>> mCollector;

        ImGuiDockNode *mDockNode = nullptr;
    };

}
}

RegisterType(Engine::Tools::ImRoot);
