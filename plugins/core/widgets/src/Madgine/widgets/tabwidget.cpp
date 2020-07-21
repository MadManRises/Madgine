#include "../widgetslib.h"

#include "tabwidget.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN_BASE(Engine::Widgets::TabWidget, Engine::Widgets::WidgetBase)
METATABLE_END(Engine::Widgets::TabWidget)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::TabWidget, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::TabWidget)

namespace Engine
{
namespace Widgets
	{
		void TabWidget::addTab(Widget * w)
		{
		}

		    WidgetClass TabWidget::getClass() const
                {
                    return WidgetClass::TABWIDGET_CLASS;
                }
	}
}