#include "../../clientlib.h"

#include "checkbox.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::GUI::Checkbox)
METATABLE_END(Engine::GUI::Checkbox)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::Checkbox, Engine::GUI::WidgetBase)
SERIALIZETABLE_END(Engine::GUI::Checkbox)

namespace Engine
{
	namespace GUI
	{
		bool Checkbox::isChecked()
		{
			return false;
		}
		void Checkbox::setChecked(bool b)
		{
		}
                WidgetClass Checkbox::getClass() const
                {
                    return WidgetClass::CHECKBOX_CLASS;
                }
	}
}