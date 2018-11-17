#include "../../clientlib.h"

#include "label.h"

#include "Interfaces/scripting/types/api.h"

#include "Interfaces/generic/keyvalueiterate.h"



namespace Engine
{
	namespace GUI {

		KeyValueMapList Label::maps()
		{
			return Scope::maps().merge(MAP(Text, getText, setText));
		}

	}
}
