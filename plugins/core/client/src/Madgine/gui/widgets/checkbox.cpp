#include "../../clientlib.h"

#include "checkbox.h"

#include "Modules/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::GUI::Checkbox)
METATABLE_END(Engine::GUI::Checkbox)

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
	}
}