#include "directinputlib.h"

#include "directinputhandler.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/math/vector2.h"

#include "Madgine/input/inputevents.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Input::DirectInputHandler);

namespace Engine {
namespace Input {


    DirectInputHandler::DirectInputHandler(Window::MainWindow &topLevel, Window::OSWindow *window, InputListener *listener)
        : UniqueComponent(topLevel, listener)
    {
    }

    DirectInputHandler::~DirectInputHandler()
    {
    }

    bool DirectInputHandler::isKeyDown(Key::Key key)
    {
        return false;
    }

    bool DirectInputHandler::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        PROFILE();

        return true;
    }

}
}



METATABLE_BEGIN(Engine::Input::DirectInputHandler)
METATABLE_END(Engine::Input::DirectInputHandler)