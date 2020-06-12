#include "../clientlib.h"

#include "inputhandler.h"

#include "inputlistener.h"

#include "../window/mainwindow.h"

namespace Engine {
namespace Input {

    InputHandler::InputHandler(Window::MainWindow &window, InputListener *listener)
        : mListener(listener)
        , mWindow(window)
    {
        window.addFrameListener(this);
    }

    InputHandler::~InputHandler()
    {
        mWindow.removeFrameListener(this);
    }

    /*InputListener* InputHandler::listener()
		{
			return mListener;
		}*/

    void InputHandler::onResize(size_t width, size_t height)
    {
    }

    void InputHandler::injectKeyPress(const KeyEventArgs &arg)
    {
        if (mListener)
            mListener->injectKeyPress(arg);
    }

    void InputHandler::injectKeyRelease(const KeyEventArgs &arg)
    {
        if (mListener)
            mListener->injectKeyRelease(arg);
    }

    void InputHandler::injectPointerPress(const PointerEventArgs &arg)
    {
        if (mListener)
            mListener->injectPointerPress(arg);
    }

    void InputHandler::injectPointerRelease(const PointerEventArgs &arg)
    {
        if (mListener)
            mListener->injectPointerRelease(arg);
    }

    void InputHandler::injectPointerMove(const PointerEventArgs &arg)
    {
        if (mListener)
            mListener->injectPointerMove(arg);
    }

}
}

RegisterType(Engine::Input::InputHandler);