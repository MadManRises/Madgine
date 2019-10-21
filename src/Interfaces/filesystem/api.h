#pragma once

#include "../streams/streams.h"
#include "filequery.h"

namespace Engine {
namespace Filesystem {

	struct FileInfo {
        size_t mSize;
	};

    INTERFACES_EXPORT FileQuery listFilesRecursive(const Path &path);
    INTERFACES_EXPORT FileQuery listFiles(const Path &path);
    INTERFACES_EXPORT FileQuery listFolders(const Path &path);
    INTERFACES_EXPORT Path configPath();

    INTERFACES_EXPORT Path makeNormalized(const char *p);

    INTERFACES_EXPORT Path getCwd();
    INTERFACES_EXPORT void setCwd(const Path &p);
    INTERFACES_EXPORT void createDirectory(const Path &p);
    INTERFACES_EXPORT void createDirectories(const Path &p);
    INTERFACES_EXPORT bool exists(const Path &p);
    INTERFACES_EXPORT bool remove(const Path &p);
    INTERFACES_EXPORT bool isAbsolute(const Path &p);
    INTERFACES_EXPORT bool isSeparator(char c);
    INTERFACES_EXPORT bool isEqual(const Path &p1, const Path &p2);

    INTERFACES_EXPORT InStream openFile(const Path &p, bool isBinary = false);
    INTERFACES_EXPORT FileInfo fileInfo(const Path &p);
}
}