#pragma once

#include <stddef.h>

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

#ifndef STATIC_BUILD
		extern "C" const BinaryInfo PLUGIN_LOCAL(binaryInfo);
#endif

	}
}