#include "../../widgetslib.h"

#include "widgetsenders.h"

#include "Madgine/behaviorerror.h"

#include "Madgine/nativebehaviorcollector.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

NATIVE_BEHAVIOR(Get_Widget, Engine::Widgets::get_widget)
NATIVE_BEHAVIOR(Get_WidgetManager, Engine::Widgets::get_widget_manager)

NATIVE_BEHAVIOR(Widget_PointerClick, Engine::Widgets::widget_pointer_click_sender)