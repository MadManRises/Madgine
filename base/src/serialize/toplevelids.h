#pragma once

#include "serialize/serializemanager.h"

namespace Engine {
	namespace Serialize {

		enum TopLevelIds {
			SCENE_MANAGER = BEGIN_USER_ID_SPACE,
			UTIL,
			SCRIPTING_MANAGER,
			BEGIN_MADGINE_USER_ID_SPACE
		};

	}
}