#pragma once

#include "../ois/includes/OIS.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Madgine/input/inputcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Input {
    struct MADGINE_OIS_EXPORT OISInputHandler : OIS::KeyListener, OIS::MouseListener, InputHandlerComponent<OISInputHandler> {
        OISInputHandler(GUI::TopLevelWindow &topLevel, Window::Window *window, InputListener *listener);
        virtual ~OISInputHandler();

        virtual bool isKeyDown(Key key) override;

    private:
        static MouseButton::MouseButton convertMouseButton(OIS::MouseButtonID id);

        // Inherited via KeyListener
        bool keyPressed(const OIS::KeyEvent &arg) override;
        bool keyReleased(const OIS::KeyEvent &arg) override;

        // Inherited via MouseListener
        bool mouseMoved(const OIS::MouseEvent &arg) override;
        bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;
        bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;

        bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        void onResize(size_t width, size_t height) override;

        float mMouseScale;

        //OIS Input devices
        OIS::InputManager *mInputManager;
        OIS::Mouse *mMouse;
        OIS::Keyboard *mKeyboard;

        Window::Window *mWindow;
    };
}
}
