#include "../../clientlib.h"

#include "tabwidget.h"

#include "Modules/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::GUI::TabWidget)
METATABLE_END(Engine::GUI::TabWidget)

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