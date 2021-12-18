#include "../playercontrolslib.h"

#include "playercontroller.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/controls/controlsmanager.h"

#include "Interfaces/input/inputevents.h"

#include "Madgine/physics/rigidbody.h"

SERIALIZETABLE_INHERIT_BEGIN(Engine::Input::PlayerController, Engine::Scene::SceneComponentBase)
FIELD(mTarget)
SERIALIZETABLE_END(Engine::Input::PlayerController)

METATABLE_BEGIN_BASE(Engine::Input::PlayerController, Engine::Scene::SceneComponentBase)
MEMBER(mTarget)
METATABLE_END(Engine::Input::PlayerController)

UNIQUECOMPONENT(Engine::Input::PlayerController)

namespace Engine {
namespace Input {

    PlayerController::PlayerController(Scene::SceneManager &scene)
        : VirtualScope(scene)
    {
    }

    bool PlayerController::init()
    {
        sceneMgr().app().getGlobalAPIComponent<ControlsManager>().addAxisEventListener(this);

        return VirtualScope::init();
    }

    void PlayerController::finalize()
    {
        VirtualScope::finalize();

        sceneMgr().app().getGlobalAPIComponent<ControlsManager>().removeAxisEventListener(this);        
    }

    bool PlayerController::onAxisEvent(const AxisEventArgs &arg)
    {
        if (arg.mAxisType == AxisEventArgs::LEFT) {
            if (mTarget)
                mTarget->getComponent<Physics::RigidBody>()->setVelocity({ 3.0f * arg.mAxis1, 0.0f, -3.0f * arg.mAxis2 });
        }
        return false;
    }

}
}
