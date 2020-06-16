#pragma once

#include "../threading/framelistener.h"


namespace Engine {
namespace Input {
    struct MADGINE_CLIENT_EXPORT InputHandler : Threading::FrameListener {    
        InputHandler(Window::MainWindow &window, InputListener *listener);
        virtual ~InputHandler();

        //InputListener *listener();

        virtual void onResize(size_t width, size_t height);

		virtual bool isKeyDown(Key key) = 0;

    protected:
        bool injectKeyPress(const KeyEventArgs &arg);
        bool injectKeyRelease(const KeyEventArgs &arg);
        bool injectPointerPress(const PointerEventArgs &arg);
        bool injectPointerRelease(const PointerEventArgs &arg);
        bool injectPointerMove(const PointerEventArgs &arg);

    private:
        InputListener *mListener;
        Window::MainWindow &mWindow;
    };
}
}

RegisterType(Engine::Input::InputHandler);