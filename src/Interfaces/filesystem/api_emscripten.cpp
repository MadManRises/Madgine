#include "../interfaceslib.h"

#if EMSCRIPTEN

#    include "api.h"

#    include <dirent.h>
#    include <sys/stat.h>
#    include <unistd.h>

namespace Engine {
namespace Filesystem {

    Path executablePath()
    {
        return Path { "/cwd" };
    }

    std::string executableName()
    {
        return "MadgineExecutable";
    }

    Path appDataPath()
    {
        return Path { "/cwd" };
    }

    bool isDir(const Path &p)
    {
        struct stat statbuffer;
        auto result = stat(p.c_str(), &statbuffer);
        assert(result != -1);
        return (statbuffer.st_mode & S_IFMT) == S_IFDIR;
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

    void makeNormalized(Path &p)
    {
    }

    bool isAbsolute(const Path &p)
    {
        return p.c_str()[0] == '/';
    }

    bool isEqual(const Path &p1, const Path &p2)
    {
        return p1 == p2;
    }

    InStream openFileRead(const Path &p, bool isBinary)
    {
        std::unique_ptr<std::filebuf> buffer = std::make_unique<std::filebuf>();
        if (buffer->open(p.c_str(), static_cast<std::ios_base::openmode>(std::ios_base::in | (isBinary ? std::ios_base::binary : 0))))
            return { std::move(buffer) };
        else
            return {};
    }

    OutStream openFileWrite(const Path &p, bool isBinary)
    {
        std::unique_ptr<std::filebuf> buffer = std::make_unique<std::filebuf>();
        if (buffer->open(p.c_str(), static_cast<std::ios_base::openmode>(std::ios_base::out | (isBinary ? std::ios_base::binary : 0))))
            return { std::move(buffer) };
        else
            return {};
    }

    bool isValidPath(const std::string &p)
    {
        for (char c : p)
            if ((!std::isalnum(c) && !isSeparator(c) && !std::ispunct(c) && c != ' ')
                || c == '<'
                || c == '>'
                || c == ':'
                || c == '"'
                || c == '|'
                || c == '?'
                || c == '*')
                return false;
        return true;
    }

}
}

#endif