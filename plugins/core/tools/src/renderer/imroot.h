#pragma once

#include "../toolscollector.h"

#include "Modules/madgineobject/madgineobject.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ImRoot : MadgineObject, ScopeBase {
        ImRoot(const MadgineObject *parent);
        ~ImRoot();

        bool init() override;
        void finalize() override;

        const ToolsContainer<std::vector> &tools();
        ToolBase &getToolComponent(size_t index, bool = true);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(::Engine::component_index<T>()));
        }

        bool frame();

        virtual const MadgineObject *parent() const override;

    private:
        const MadgineObject *mParent; //TODO Find proper solution
		
		ToolsContainer<std::vector> mCollector;		
    };

}
}
