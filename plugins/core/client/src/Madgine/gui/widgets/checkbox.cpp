#include "../../clientlib.h"

#include "checkbox.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::Checkbox)
METATABLE_END(Engine::Widgets::Checkbox)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Checkbox, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::Checkbox)

namespace Engine
{
namespace Widgets
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