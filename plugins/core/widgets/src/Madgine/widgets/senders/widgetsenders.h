#pragma once

#include "Generic/execution/algorithm.h"
#include "Madgine/bindings.h"

#include "../widget.h"

namespace Engine {
namespace Widgets {

    constexpr auto get_widget = get_binding_sender<"Widget", WidgetBase *>;

    constexpr auto get_widget_manager = get_binding_sender<"WidgetManager", WidgetManager *>;

}
}
