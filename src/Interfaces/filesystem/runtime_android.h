#pragma once

#if ANDROID

struct AAssetManager;

namespace Engine {
	namespace Filesystem {

		INTERFACES_EXPORT void setAndroidAssetManager(AAssetManager *runtimePath);

	}
}

#endif