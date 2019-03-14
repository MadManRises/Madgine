#pragma once

#include "filequery.h"

namespace Engine {
	namespace Filesystem {

		INTERFACES_EXPORT FileQuery listFilesRecursive(const Path &path);
		INTERFACES_EXPORT Path configPath();

		INTERFACES_EXPORT Path makeNormalized(const char *p);

		INTERFACES_EXPORT void createDirectory(const Path &p);
		INTERFACES_EXPORT void createDirectories(const Path &p);
		INTERFACES_EXPORT bool exists(const Path &p);
		INTERFACES_EXPORT bool remove(const Path &p);
		INTERFACES_EXPORT bool isAbsolute(const Path &p);
		INTERFACES_EXPORT bool isSeparator(char c);

		INTERFACES_EXPORT std::vector<char> readFile(const Path &p);

	}
}