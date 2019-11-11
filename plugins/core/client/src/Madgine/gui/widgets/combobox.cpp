#include "../../clientlib.h"

#include "combobox.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::Combobox)
METATABLE_END(Engine::Widgets::Combobox)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Combobox, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::Combobox)

namespace Engine
{
namespace Widgets
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