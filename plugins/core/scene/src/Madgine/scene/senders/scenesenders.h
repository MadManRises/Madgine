#pragma once

#include "Generic/execution/algorithm.h"
#include "Madgine/bindings.h"

#include "../scenemanager.h"

#include "../entity/components/transform.h"


#include "Madgine/nativebehaviorcollector.h"

namespace Engine {
namespace Scene {

    constexpr auto get_entity = get_binding_sender<"Entity", Entity::Entity *>;

    constexpr auto get_scene = get_binding_sender<"Scene", SceneManager *>;

    constexpr auto wait_simulation = [](std::chrono::steady_clock::duration duration) {
        return IntervalClock<Threading::CustomTimepoint>::wait(get_scene() | Execution::then(&SceneManager::simulationClock), duration);
    };

    constexpr auto yield_simulation = []() {
        return wait_simulation(0s);
    };

    template <typename T>
    constexpr auto get_component = []() { return get_entity() | Execution::then([](Entity::Entity *e) { return e->getComponent<T>(); }); };

    constexpr auto rotate = [](Vector3 axis, float speed) {
        return yield_simulation() | Execution::let_value([=](std::chrono::microseconds timeSinceLastFrame) {
            return get_component<Entity::Transform>() | Execution::then([=](Entity::Transform *transform) {
                transform->mOrientation *= Quaternion { timeSinceLastFrame.count() * 0.000001f * speed, axis };
            });
        }) | Execution::repeat;
    };

}
}

NATIVE_BEHAVIOR_DECLARATION(Yield_Simulation)
NATIVE_BEHAVIOR_DECLARATION(Wait_Simulation)

NATIVE_BEHAVIOR_DECLARATION(Rotate)