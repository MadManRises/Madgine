#include "../../clientlib.h"

#include "label.h"

#include "../../scripting/types/api.h"

#include "../../generic/keyvalueiterate.h"

RegisterClass(Engine::GUI::Label);

namespace Engine
{
	namespace GUI {

		KeyValueMapList Label::maps()
		{
			return Scope::maps().merge(MAP(Text, getText, setText));
		}

	}
}
