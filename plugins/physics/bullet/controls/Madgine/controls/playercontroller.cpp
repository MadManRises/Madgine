#include "../playercontrolslib.h"

#include "playercontroller.h"

#include "Modules//keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/controls/controlsmanager.h"

#include "Interfaces/input/inputevents.h"

#include "Madgine/physics/rigidbody.h"

SERIALIZETABLE_INHERIT_BEGIN(Engine::Controls::PlayerController, Engine::Scene::SceneComponentBase)
FIELD(mTarget)
SERIALIZETABLE_END(Engine::Controls::PlayerController)

METATABLE_BEGIN_BASE(Engine::Controls::PlayerController, Engine::Scene::SceneComponentBase)
MEMBER(mTarget)
METATABLE_END(Engine::Controls::PlayerController)

UNIQUECOMPONENT(Engine::Controls::PlayerController)


    namespace Engine
{
    namespace Controls {

        PlayerController::PlayerController(Scene::SceneManager &scene)
            : VirtualScope(scene)
        {
        }

        bool PlayerController::init()
        {
            sceneMgr().app().getGlobalAPIComponent<Controls::ControlsManager>().addAxisEventListener(this);

            return VirtualScope::init();
        }

        bool PlayerController::onAxisEvent(const Input::AxisEventArgs &arg)
        {
            if (arg.mAxisType == Input::AxisEventArgs::LEFT) {
                if (mTarget)
                    mTarget->getComponent<Physics::RigidBody>()->setVelocity({ 3.0f * arg.mAxis1, 0.0f, -3.0f * arg.mAxis2 });
            }
            return false;
        }

    }
}
