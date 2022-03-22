#pragma once

#include "Madgine/scene/scenecomponentbase.h"
#include "Madgine/scene/scenecomponentcollector.h"

#include "Madgine/controls/axiseventlistener.h"

#include "Madgine/scene/entity/entityptr.h"

namespace Engine {
namespace Input {

    struct MADGINE_PLAYERCONTROLS_EXPORT PlayerController : Scene::SceneComponent<PlayerController>, AxisEventListener {

        PlayerController(Scene::SceneManager &scene);

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual bool onAxisEvent(const AxisEventArgs &arg) override;

        Scene::Entity::EntityPtr mTarget;
    };

}
}

RegisterType(Engine::Input::PlayerController)