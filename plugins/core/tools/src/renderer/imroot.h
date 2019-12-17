#pragma once

#include "../toolscollector.h"

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/observablecontainer.h"

#include "Modules/madgineobject/madgineobjectobserver.h"

struct ImGuiDockNode;

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ImRoot : MadgineObject, ScopeBase {
        ImRoot(const MadgineObject *parent);
        ~ImRoot();

        bool init() override;
        void finalize() override;

        const std::vector<std::unique_ptr<ToolBase>> &tools();
        ToolBase &getToolComponent(size_t index, bool = true);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(::Engine::component_index<T>()));
        }

        bool frame();

        virtual const MadgineObject *parent() const override;

		ImGuiDockNode *dockNode() const;

    private:
        const MadgineObject *mParent; //TODO Find proper solution
		
		ToolsContainer<ObservableContainer<std::vector<Placeholder<0>>, MadgineObjectObserver>> mCollector;		

		ImGuiDockNode *mDockNode = nullptr;
    };

}
}
