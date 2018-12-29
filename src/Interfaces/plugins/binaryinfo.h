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


		};

#if !defined(STATIC_BUILD) && defined(PROJECT_NAME)
#	define PLUGIN_LOCAL(name) CONCAT2(CONCAT(name, _), PROJECT_NAME)
		extern "C" const BinaryInfo PLUGIN_LOCAL(binaryInfo);
#endif

	}
}