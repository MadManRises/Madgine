#pragma once

#include "Generic/execution/algorithm.h"
#include "Madgine/state.h"

#include "../widget.h"

#include "Interfaces/input/inputevents.h"

namespace Engine {
namespace Widgets {

    constexpr auto get_widget = Execution::read_var<"Widget", WidgetBase *>;

    constexpr auto get_widget_manager = Execution::read_var<"WidgetManager", WidgetManager *>;

    constexpr auto widget_pointer_click_sender = []() {
        return get_widget() | Execution::then([](WidgetBase *w) {
            return w->pointerClickEvent().sender() | Execution::then([](const Input::PointerEventArgs &args) {
                return 3;
            });
        });
    };

    constexpr auto widget_pointer_enter_sender = []() {
        return get_widget() | Execution::then([](WidgetBase *w) {
            return w->pointerEnterEvent().sender() | Execution::then([](const Input::PointerEventArgs &args) {
                return 3;
            });
        });
    };

    constexpr auto widget_pointer_leave_sender = []() {
        return get_widget() | Execution::then([](WidgetBase *w) {
            return w->pointerLeaveEvent().sender() | Execution::then([](const Input::PointerEventArgs &args) {
                return 3;
            });
        });
    };

}
}