#pragma once

#include <stddef.h>

#include "../macros.h"

namespace Engine {
	namespace Plugins {

		struct BinaryInfo {

			size_t mMajorVersion;
			size_t mMinorVersion;
			size_t mPathNumber;

			const char *mName;

			const char *mProjectRoot;
			const char *mSourceRoot;
			const char *mBinaryDir;
			const char *mBinaryFileName;

			const char *mPrecompiledHeaderPath;

			const char **mPluginDependencies;

		};

#if !defined(STATIC_BUILD)
#	if defined(PROJECT_NAME)
#		define PLUGIN_LOCAL(name) CONCAT2(CONCAT(name, _), PROJECT_NAME)
#	else
#		define PLUGIN_LOCAL(name) __UNDEFINED__ ## name
#	endif
		extern "C" const BinaryInfo PLUGIN_LOCAL(binaryInfo);
#endif

	}
}