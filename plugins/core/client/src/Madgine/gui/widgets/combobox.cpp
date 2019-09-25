#include "../../clientlib.h"

#include "combobox.h"

#include "Modules/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::GUI::Combobox)
METATABLE_END(Engine::GUI::Combobox)

namespace Engine
{
	namespace GUI
	{
		void Combobox::addItem(const std::string & text)
		{
		}
		void Combobox::clear()
		{
		}
		void Combobox::setText(const std::string & s)
		{
		}
		std::string Combobox::getText()
		{
			return std::string();
		}
                WidgetClass Combobox::getClass() const
                {
                    return WidgetClass::COMBOBOX_CLASS;
                }
	}
}