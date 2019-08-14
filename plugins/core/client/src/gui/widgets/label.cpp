#include "../../clientlib.h"

#include "label.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"


namespace Engine
{
	namespace GUI {
		std::string Label::getText()
		{
			return std::string();
		}
		void Label::setText(const std::string & text)
		{
		}
		/*KeyValueMapList Label::maps()
		{
			return Scope::maps().merge(MAP(Text, getText, setText));
		}*/

	}
}


METATABLE_BEGIN(Engine::GUI::Label)
METATABLE_END(Engine::GUI::Label)

RegisterType(Engine::GUI::Label);