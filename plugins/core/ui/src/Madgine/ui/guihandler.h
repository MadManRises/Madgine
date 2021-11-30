#pragma once

#include "handler.h"

#include "handlercollector.h"

namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT GuiHandlerBase : Handler {    
        enum class WindowType {
            MODAL_OVERLAY,
            NONMODAL_OVERLAY,
            ROOT_WINDOW
        };

        GuiHandlerBase(UIManager &ui, WindowType type);

        virtual void open();
        virtual void close();
        bool isOpen() const;

        bool isRootWindow() const;

    private:
        const WindowType mType;
    };
}
}

RegisterType(Engine::UI::GuiHandlerBase);

