#pragma once

#include "Modules/math/vector2.h"

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Madgine/render/renderpass.h"

struct ImGuiDockNode;
struct ImGuiViewport;

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ImManager : Render::RenderPass {
        ImManager(GUI::TopLevelWindow &window);
        virtual ~ImManager();

        virtual void newFrame(float timeSinceLastFrame) = 0;
        virtual void render(Render::RenderTarget *target) override;

        void addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp);
        void removeViewportMapping(Render::RenderTarget *target);

    protected:
        GUI::TopLevelWindow &mWindow;

        std::map<Render::RenderTarget *, ImGuiViewport *> mViewportMappings;
    };

}
}

DECLARE_UNIQUE_COMPONENT(Engine::Tools, ImManager, ImManager, GUI::TopLevelWindow &);