#pragma once

#include "../threading/framelistener.h"

#include "inputevents.h"

namespace Engine {
namespace Input {
    struct MADGINE_CLIENT_EXPORT InputHandler : Threading::FrameListener {    
        InputHandler(Window::MainWindow &window, InputListener *listener);
        virtual ~InputHandler();

        //InputListener *listener();

        virtual void onResize(size_t width, size_t height);

		virtual bool isKeyDown(Key key) = 0;

    protected:
        void injectKeyPress(const KeyEventArgs &arg);
        void injectKeyRelease(const KeyEventArgs &arg);
        void injectPointerPress(const PointerEventArgs &arg);
        void injectPointerRelease(const PointerEventArgs &arg);
        void injectPointerMove(const PointerEventArgs &arg);

    private:
        InputListener *mListener;
        Window::MainWindow &mWindow;
    };
}
}

RegisterType(Engine::Input::InputHandler);