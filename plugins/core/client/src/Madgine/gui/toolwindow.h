#pragma once

#include "../input/inputcollector.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/keyvalue/scopebase.h"

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

        Render::RenderTarget *getRenderer();

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(size_t width, size_t height) override;

    private:

        std::optional<Input::InputHandlerSelector> mInputHandlerSelector;

        Window::Window *mWindow = nullptr;
        std::unique_ptr<Render::RenderTarget> mRenderWindow;

    };

}
}
