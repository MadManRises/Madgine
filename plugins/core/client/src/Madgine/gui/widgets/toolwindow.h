#pragma once

#include "../../input/inputcollector.h"
#include "../../input/inputlistener.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/generic/transformIt.h"

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/scopebase.h"

#include "../../render/renderwindowcollector.h"

namespace Engine {
namespace GUI {

    class MADGINE_CLIENT_EXPORT ToolWindow : public ScopeBase, public Window::WindowEventListener {

    public:
        ToolWindow(TopLevelWindow &parent, const Window::WindowSettings &settings);
        virtual ~ToolWindow();

        void close();

        Vector3 getScreenSize();

        Input::InputHandler *input();

        Window::Window *window();

        Render::RenderWindow *getRenderer();

		void beginFrame();
        void endFrame();

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(size_t width, size_t height) override;

    private:
        TopLevelWindow &mParent;

        std::optional<Input::InputHandlerSelector> mInputHandlerSelector;

        Window::Window *mWindow = nullptr;
        std::optional<Render::RenderWindowSelector> mRenderWindow;

    };

}
}
