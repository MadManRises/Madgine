#pragma once

#include "Madgine/scene/scenecomponentcollector.h"
#include "Madgine/scene/scenecomponentbase.h"

#include "Madgine/controls/axiseventlistener.h"

#include "Madgine/scene/entity/entityptr.h"

namespace Engine {
namespace Input {

    struct MADGINE_PLAYERCONTROLS_EXPORT PlayerController : Scene::SceneComponent<PlayerController>, AxisEventListener {

        PlayerController(Scene::SceneManager &scene);

        virtual bool init() override;
        virtual void finalize() override;

        virtual bool onAxisEvent(const AxisEventArgs &arg) override;

        Scene::Entity::EntityPtr mTarget;
    };

}
}

RegisterType(Engine::Input::PlayerController)