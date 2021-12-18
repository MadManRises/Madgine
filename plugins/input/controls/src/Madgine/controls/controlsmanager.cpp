#include "../controlslib.h"

#include "controlsmanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "axiseventlistener.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Input::ControlsManager)

METATABLE_BEGIN_BASE(Engine::Input::ControlsManager, Engine::App::GlobalAPIBase)
METATABLE_END(Engine::Input::ControlsManager)

namespace Engine {
namespace Input {

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
        mControlsMap.try_emplace(std::string { action }).first->second.mListeners.emplace_back(ActionListener { callback, userData });
    }

    void ControlsManager::trigger(const ActionDescriptor *action)
    {
        for (const ActionListener &listener : action->mListeners) {
            listener.mF(action, listener.mUserData);
        }
    }

    void ControlsManager::addAxisEventListener(AxisEventListener *listener)
    {
        mAxisEventListeners.push_back(listener);
    }

    void ControlsManager::removeAxisEventListener(AxisEventListener *listener)
    {
        mAxisEventListeners.erase(std::find(mAxisEventListeners.begin(), mAxisEventListeners.end(), listener));
    }

    bool ControlsManager::injectAxisEvent(const AxisEventArgs &arg)
    {
        for (AxisEventListener *listener : mAxisEventListeners)
            if (listener->onAxisEvent(arg))
                return true;
        return false;
    }

}
}
