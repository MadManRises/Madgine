#include "../clientlib.h"

#include "inputhandler.h"

#include "inputlistener.h"

#include "../window/mainwindow.h"

#include "inputevents.h"

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

    bool InputHandler::injectKeyPress(const KeyEventArgs &arg)
    {
        if (!mListener)
            return false;
        return mListener->injectKeyPress(arg);
    }

    bool InputHandler::injectKeyRelease(const KeyEventArgs &arg)
    {
        if (!mListener)
            return false;
        return mListener->injectKeyRelease(arg);
    }

    bool InputHandler::injectPointerPress(const PointerEventArgs &arg)
    {
        if (!mListener)
            return false;
        return mListener->injectPointerPress(arg);
    }

    bool InputHandler::injectPointerRelease(const PointerEventArgs &arg)
    {
        if (!mListener)
            return false;
        return mListener->injectPointerRelease(arg);
    }

    bool InputHandler::injectPointerMove(const PointerEventArgs &arg)
    {
        if (!mListener)
            return false;
        return mListener->injectPointerMove(arg);
    }

}
}
