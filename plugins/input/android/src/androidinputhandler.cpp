#include "androidinputlib.h"

#include "androidinputhandler.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/math/vector2.h"

#include <android/input.h>

#include "Madgine/input/inputevents.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Input::AndroidInputHandler);

namespace Engine {
namespace Input {

    DLL_EXPORT AInputQueue *sQueue = nullptr;

    AndroidInputHandler::AndroidInputHandler(Window::MainWindow &topLevel, Window::OSWindow *window, InputListener *listener)
        : UniqueComponent(topLevel, listener)
    {
    }

    AndroidInputHandler::~AndroidInputHandler()
    {
    }

    bool AndroidInputHandler::isKeyDown(Key::Key key)
    {
        return false;
    }

    bool AndroidInputHandler::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        PROFILE();
        if (sQueue) {
            AInputEvent *event = NULL;
            while (AInputQueue_getEvent(sQueue, &event) >= 0) {
                if (AInputQueue_preDispatchEvent(sQueue, event)) {
                    continue;
                }
                bool handled = false;
                switch (AInputEvent_getType(event)) {
                case AINPUT_EVENT_TYPE_KEY:
                    //TODO
                    std::terminate();
                    break;
                case AINPUT_EVENT_TYPE_MOTION:
                    handled = handleMotionEvent(event);
                    break;
                default:
                    LOG_ERROR("Unknown Event Type: " << AInputEvent_getType(event));
                    break;
                }
                AInputQueue_finishEvent(sQueue, event, handled);
            }
        }

        return true;
    }

   

}
}



METATABLE_BEGIN(Engine::Input::AndroidInputHandler)
METATABLE_END(Engine::Input::AndroidInputHandler)