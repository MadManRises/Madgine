#include "../controlslib.h"

#include "controlsmanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "axiseventlistener.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"


UNIQUECOMPONENT(Engine::Controls::ControlsManager)

METATABLE_BEGIN_BASE(Engine::Controls::ControlsManager, Engine::App::GlobalAPIBase)
METATABLE_END(Engine::Controls::ControlsManager)

namespace Engine {
namespace Controls {
    
    ControlsManager::ControlsManager(App::Application &app)
        : VirtualScope(app)
    {
    }

    const ActionDescriptor *ControlsManager::action(std::string_view name)
    {
        return &mControlsMap.try_emplace(std::string { name }).first->second;
    }

    void ControlsManager::addListener(std::string_view action, ActionCallback callback, void *userData)
    {
        mControlsMap.try_emplace(std::string { action }).first->second.mListeners.emplace_back(ActionListener {callback, userData});
    }

    void ControlsManager::trigger(const ActionDescriptor *action)
    {
        for (const ActionListener& listener : action->mListeners) {
            listener.mF(action, listener.mUserData);
        }
    }

    void ControlsManager::addAxisEventListener(AxisEventListener *listener)
    {
        mAxisEventListeners.push_back(listener);
    }

    bool ControlsManager::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        for (AxisEventListener *listener : mAxisEventListeners)
            if (listener->onAxisEvent(arg))
                return true;
        return false;
    }

}
}
