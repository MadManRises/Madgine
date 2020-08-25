#pragma once

#include "../input/inputcollector.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/keyvalue/scopebase.h"

namespace Engine {
namespace Window {

    struct MADGINE_CLIENT_EXPORT ToolWindow : ScopeBase, WindowEventListener {    
        ToolWindow(MainWindow &parent, const WindowSettings &settings);
        virtual ~ToolWindow();

        void close();

        Vector3 getScreenSize();

        Input::InputHandler *input();

        OSWindow *osWindow();

        Render::RenderTarget *getRenderer();

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(size_t width, size_t height) override;

    private:

        std::optional<Input::InputHandlerSelector<>> mInputHandlerSelector;

        OSWindow *mOsWindow = nullptr;
        std::unique_ptr<Render::RenderTarget> mRenderWindow;

    };

}
}
