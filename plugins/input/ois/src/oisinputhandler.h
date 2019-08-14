#pragma once

#include "../ois/includes/OIS.h"
#include "Interfaces/window/windoweventlistener.h"
#include "input/inputcollector.h"

namespace Engine {
namespace Input {
    class MADGINE_OIS_EXPORT OISInputHandler : public OIS::KeyListener, public OIS::MouseListener, public InputHandlerComponent<OISInputHandler> {
    public:
        OISInputHandler(Window::Window *window, App::Application &app, InputListener *listener);
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
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

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
