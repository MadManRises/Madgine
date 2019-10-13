#include "../../clientlib.h"

#include "textbox.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::GUI::Textbox)
METATABLE_END(Engine::GUI::Textbox)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::Textbox, Engine::GUI::WidgetBase)
SERIALIZETABLE_END(Engine::GUI::Textbox)

namespace Engine
{
	namespace GUI
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