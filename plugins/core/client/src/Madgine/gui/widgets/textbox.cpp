#include "../../clientlib.h"

#include "textbox.h"

#include "Modules/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::GUI::Textbox)
METATABLE_END(Engine::GUI::Textbox)

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