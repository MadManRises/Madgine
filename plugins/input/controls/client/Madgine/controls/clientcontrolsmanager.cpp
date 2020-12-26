#include "../clientcontrolslib.h"

#include "clientcontrolsmanager.h"

#include "Madgine/app/application.h"

#include "Madgine/controls/controlsmanager.h"
#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

UNIQUECOMPONENT(Engine::Controls::ClientControlsManager)

RegisterType(Engine::Controls::ClientControlsManager)

METATABLE_BEGIN_BASE(Engine::Controls::ClientControlsManager, Engine::Window::MainWindowComponentBase)
METATABLE_END(Engine::Controls::ClientControlsManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Controls::ClientControlsManager, Engine::Window::MainWindowComponentBase)
SERIALIZETABLE_END(Engine::Controls::ClientControlsManager)

namespace Engine {
namespace Controls {

    ClientControlsManager::ClientControlsManager(Window::MainWindow &window)
        : VirtualUnit(window, 100)
        , mManager(Engine::App::Application::getSingleton().getGlobalAPIComponent<ControlsManager>())
    {
    }

    std::string_view ClientControlsManager::key() const
    {
        return "ClientControlsManager";
    }



}
}
