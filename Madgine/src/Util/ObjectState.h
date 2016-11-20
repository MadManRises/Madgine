#pragma once


namespace Engine {
	namespace Util {

		enum class ObjectState {
			CONSTRUCTED,
			INITIALIZED,
			IN_SCENE,
			IN_SCENE_ABOUT_TO_CLEAR
		};

	}
}