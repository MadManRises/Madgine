#include "../../clientlib.h"

#include "tabwidget.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::GUI::TabWidget)
METATABLE_END(Engine::GUI::TabWidget)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::TabWidget, Engine::GUI::WidgetBase)
SERIALIZETABLE_END(Engine::GUI::TabWidget)

namespace Engine
{
	namespace GUI
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