#pragma once

#include "Generic/execution/algorithm.h"
#include "Madgine/state.h"

#include "../widget.h"

namespace Engine {
namespace Widgets {

    constexpr auto get_widget = Execution::read_var<"Widget", WidgetBase *>;

    constexpr auto get_widget_manager = Execution::read_var<"WidgetManager", WidgetManager *>;

}
}
