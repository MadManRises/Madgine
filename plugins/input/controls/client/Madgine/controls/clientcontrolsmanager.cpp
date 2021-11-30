#include "../clientcontrolslib.h"

#include "clientcontrolsmanager.h"

#include "Madgine/app/application.h"

#include "Madgine/controls/controlsmanager.h"
#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Controls::ClientControlsManager)

METATABLE_BEGIN_BASE(Engine::Controls::ClientControlsManager, Engine::Window::MainWindowComponentBase)
METATABLE_END(Engine::Controls::ClientControlsManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Controls::ClientControlsManager, Engine::Window::MainWindowComponentBase)
SERIALIZETABLE_END(Engine::Controls::ClientControlsManager)

namespace Engine {
namespace Controls {

    ClientControlsManager::ClientControlsManager(Window::MainWindow &window)
        : VirtualData(window, 100)
        
    {
    }

    std::string_view ClientControlsManager::key() const
    {
        return "ClientControlsManager";
    }

    Threading::Task<bool> Engine::Controls::ClientControlsManager::init()
    {
        mManager = &Engine::App::Application::getSingleton().getGlobalAPIComponent<ControlsManager>();
        co_return true;
    }

    bool ClientControlsManager::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        return mManager->injectAxisEvent(arg);
    }



}
}
