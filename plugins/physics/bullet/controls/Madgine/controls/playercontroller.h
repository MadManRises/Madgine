#pragma once

#include "Madgine/scene/scenecomponentcollector.h"
#include "Madgine/scene/scenecomponentbase.h"

#include "Madgine/controls/axiseventlistener.h"

#include "Madgine/scene/entity/entityptr.h"

namespace Engine {
namespace Controls {

    struct MADGINE_PLAYERCONTROLS_EXPORT PlayerController : Scene::SceneComponent<PlayerController>, AxisEventListener {

        PlayerController(Scene::SceneManager &scene);

        virtual bool init() override;

        virtual bool onAxisEvent(const Input::AxisEventArgs &arg) override;

        Scene::Entity::EntityPtr mTarget;
    };

}
}

RegisterType(Engine::Controls::PlayerController)