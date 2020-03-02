#include "../interfaceslib.h"

#if EMSCRIPTEN

#    include "api.h"

#    include <dirent.h>
#    include <sys/stat.h>
#    include <unistd.h>

namespace Engine {
namespace Filesystem {

    Path configPath()
    {
        return Path("/config");
    }

    struct FileQueryState {
        dirent *mData;
        Path mPath;
    };

    static bool skipSymbolic(void *handle, FileQueryState &data)
    {
        while (strcmp(data.mData->d_name, ".") == 0 || strcmp(data.mData->d_name, "..") == 0) {
            data.mData = readdir((DIR *)handle);
            if (!data.mData)
                return false;
        }

        return true;
    }

    FileQueryHandle::FileQueryHandle(Path p, FileQueryState &data)
        : mPath(std::move(p))
        , mHandle(opendir(mPath.c_str()))
    {
        if (mHandle) {
            if (!advance(data)) {
                close();
            }
        }
    }

    void FileQueryHandle::close()
    {
        if (mHandle) {
            closedir((DIR *)mHandle);
            mHandle = nullptr;
        }
    }

    bool FileQueryHandle::advance(FileQueryState &data)
    {
        data.mData = readdir((DIR *)mHandle);
        data.mPath = mPath;
        if (!data.mData)
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
        struct stat statbuffer;
        auto result = stat((data.mPath / data.mData->d_name).c_str(), &statbuffer);
        assert(result != -1);
        return (statbuffer.st_mode & S_IFMT) == S_IFDIR;
    }

    const char *filename(const FileQueryState &data)
    {
        return data.mData->d_name;
    }

    void setCwd(const Path &p)
    {
        auto result = chdir(p.c_str());
        assert(result == 0);
    }

    Path getCwd()
    {
        char buffer[256];
        auto result = getcwd(buffer, sizeof(buffer));
        assert(result);
        return buffer;
    }

    void createDirectory(const Path &p)
    {
        auto result = mkdir(p.c_str(), 0700);
        assert(result == 0);
    }

    bool exists(const Path &p)
    {
        return access(p.c_str(), F_OK) != -1;
    }

    bool remove(const Path &p)
    {
        return ::remove(p.c_str());
    }

    Path makeNormalized(const char *p)
    {
        return p;
    }

    bool isAbsolute(const Path &p)
    {
        return p.c_str()[0] == '/';
    }

    bool isEqual(const Path &p1, const Path &p2)
    {
        return p1 == p2;
    }

    InStream openFile(const Path &p, bool isBinary)
    {
        std::unique_ptr<std::filebuf> buffer = std::make_unique<std::filebuf>();
        if (buffer->open(p.c_str(), std::ios_base::in | (isBinary ? std::ios_base::binary : 0)))
            return { std::move(buffer) };
        else
            return {};
    }

}
}

#endif