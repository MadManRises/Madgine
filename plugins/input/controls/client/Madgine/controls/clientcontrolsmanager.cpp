#include "../clientcontrolslib.h"

#include "clientcontrolsmanager.h"

#include "Madgine/app/application.h"

#include "Madgine/controls/controlsmanager.h"
#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Input::ClientControlsManager)

METATABLE_BEGIN_BASE(Engine::Input::ClientControlsManager, Engine::Window::MainWindowComponentBase)
METATABLE_END(Engine::Input::ClientControlsManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Input::ClientControlsManager, Engine::Window::MainWindowComponentBase)
SERIALIZETABLE_END(Engine::Input::ClientControlsManager)

namespace Engine {
namespace Input {

    ClientControlsManager::ClientControlsManager(Window::MainWindow &window)
        : VirtualData(window, 100)
        
    {
    }

    std::string_view ClientControlsManager::key() const
    {
        return "ClientControlsManager";
    }

    Threading::Task<bool> ClientControlsManager::init()
    {
        mManager = &Engine::App::Application::getSingleton().getGlobalAPIComponent<ControlsManager>();
        co_return true;
    }

    bool ClientControlsManager::injectAxisEvent(const AxisEventArgs &arg)
    {
        return mManager->injectAxisEvent(arg);
    }



}
}
