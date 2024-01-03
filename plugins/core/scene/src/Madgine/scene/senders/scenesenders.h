#pragma once

#include "Generic/execution/algorithm.h"
#include "Generic/execution/state.h"

#include "../scenemanager.h"

namespace Engine {
namespace Scene {

    constexpr auto get_entity = Execution::read_var<"Entity", Entity::Entity *>;

    constexpr auto get_scene = Execution::read_var<"Scene", SceneManager *>;    

    constexpr auto wait_simulation = [](std::chrono::steady_clock::duration duration) {
        return IntervalClock<Threading::CustomTimepoint>::wait(get_scene() | Execution::then(&SceneManager::simulationClock), duration);
    };

    constexpr auto yield_simulation = []() {
        return wait_simulation(0s);
    };

}
}