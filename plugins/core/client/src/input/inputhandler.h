#pragma once

#include "Modules/reflection/classname.h"
#include "Modules/threading/framelistener.h"

#include "inputevents.h"

namespace Engine {
namespace Input {
    class MADGINE_CLIENT_EXPORT InputHandler : public Threading::FrameListener {
    public:
        InputHandler(App::Application &app, InputListener *listener);
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
        App::Application &mApp;
    };
}
}