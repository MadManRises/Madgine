#include "../widgetslib.h"

#include "textbox.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Textbox, Engine::Widgets::WidgetBase)
METATABLE_END(Engine::Widgets::Textbox)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Textbox, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::Textbox)

namespace Engine
{
namespace Widgets
	{
		std::string Textbox::getText()
		{
			return std::string();
		}
		void Textbox::setText(const std::string & text)
		{
		}
		void Textbox::setEditable(bool b)
		{
		}
                WidgetClass Textbox::getClass() const
                {
                    return WidgetClass::TEXTBOX_CLASS;
                }
	}
}