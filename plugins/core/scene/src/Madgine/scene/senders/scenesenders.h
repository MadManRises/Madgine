#pragma once

#include "Generic/execution/state.h"

namespace Engine {
namespace Scene {

	constexpr Execution::read_var_t::Inner<"Entity", Entity::Entity *> get_entity;

}
}