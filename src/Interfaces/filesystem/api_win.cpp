#include "../interfaceslib.h"

#if WINDOWS

#    include "api.h"
#    include "path.h"

#    define NOMINMAX
#    include <Windows.h>

#    include <cctype>

#    include <direct.h>

namespace Engine {
namespace Filesystem {

    struct FileQueryState {
        WIN32_FIND_DATA mData;
    };

    static bool skipSymbolic(void *handle, FileQueryState &data)
    {
        while (strcmp(data.mData.cFileName, ".") == 0 || strcmp(data.mData.cFileName, "..") == 0)
            if (!FindNextFile(handle, &data.mData))
                return false;
        return true;
    }

    FileQueryHandle::FileQueryHandle(Path p, FileQueryState &data)
        : mPath(std::move(p))
        , mHandle(FindFirstFile((mPath.str() + "/*").c_str(), &data.mData))
    {
        if (mHandle == INVALID_HANDLE_VALUE)
            mHandle = nullptr;
        else if (!skipSymbolic(mHandle, data))
            close();
    }

    void FileQueryHandle::close()
    {
        if (mHandle) {
            FindClose(mHandle);
            mHandle = nullptr;
        }
    }

    bool FileQueryHandle::advance(FileQueryState &data)
    {
        if (!FindNextFile(mHandle, &data.mData))
            return false;
        return skipSymbolic(mHandle, data);
    }

    FileQueryState *createQueryState()
    {
        return new FileQueryState;
    }

    void destroyQueryState(FileQueryState *state)
    {
        delete state;
    }

    bool isDir(const FileQueryState &data)
    {
        return (data.mData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    const char *filename(const FileQueryState &data)
    {
        return data.mData.cFileName;
    }

    Path configPath()
    {
        char buffer[512];

        auto result = GetModuleFileName(nullptr, buffer, sizeof(buffer));
        assert(result > 0);

        return Path(buffer).parentPath();
    }

    void createDirectory(const Path &p)
    {
        auto result = CreateDirectory(p.c_str(), NULL);
        assert(result);
    }

    bool exists(const Path &p)
    {
        return GetFileAttributes(p.c_str()) != INVALID_FILE_ATTRIBUTES;
    }

    bool remove(const Path &p)
    {
        return DeleteFile(p.c_str());
    }

    Path makeNormalized(const char *p)
    {
        char buffer[512];
        auto result = GetLongPathNameA(p, buffer, sizeof(buffer));
        assert(result > 0 && result < sizeof(buffer));
        buffer[0] = toupper(buffer[0]);
        return { buffer };
    }

    bool isAbsolute(const Path &p)
    {
        auto colon = p.str().find(':');
        if (colon == std::string::npos)
            return false;
        auto sep = p.str().find('/');
        return sep == std::string::npos || colon < sep;
    }

    static bool compareChar(char c1, char c2)
    {
        return std::toupper(c1) == std::toupper(c2);
    }

    bool isEqual(const Path &p1, const Path &p2)
    {
        return ((p1.str().size() == p2.str().size()) && std::equal(p1.str().begin(), p1.str().end(), p2.str().begin(), &compareChar));
    }

    InStream openFile(const Path &p, bool isBinary)
    {
        std::unique_ptr<std::filebuf> buffer = std::make_unique<std::filebuf>();
        if (buffer->open(p.c_str(), std::ios_base::in | (isBinary ? std::ios_base::binary : 0)))
            return { std::move(buffer) };
        else
            return {};
    }

    void setCwd(const Path &p)
    {
        auto result = _chdir(p.c_str());
        assert(result == 0);
    }

    Path getCwd()
    {
        char buffer[256];
        auto result = _getcwd(buffer, sizeof(buffer));
        assert(result);
        return buffer;
    }

    FileInfo fileInfo(const Path &path)
    {
        struct _stat64 stats;
        _stat64(path.c_str(), &stats);
        return {
            static_cast<size_t>(stats.st_size)
        };
    }

}
}

#endif