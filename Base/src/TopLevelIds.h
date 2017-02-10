#pragma once

#include "Serialize\serializemanager.h"

namespace Engine {

	enum ObjectIds {
		SCENE_MANAGER = Serialize::BEGIN_USER_ID_SPACE,
		UTIL,
		SCRIPTING_MANAGER
	};

}