#pragma once

#include "Generic/execution/algorithm.h"
#include "Madgine/state.h"

#include "../widget.h"

#include "Interfaces/input/inputevents.h"

#include "Madgine/nativebehaviorcollector.h"

namespace Engine {
namespace Widgets {

    constexpr auto get_widget = Execution::read_var<"Widget", WidgetBase *>;

    constexpr auto get_widget_manager = Execution::read_var<"WidgetManager", WidgetManager *>;

}
}

NATIVE_BEHAVIOR_DECLARATION(Get_Widget)
NATIVE_BEHAVIOR_DECLARATION(Get_WidgetManager)