#include "../interfaceslib.h"

#if UNIX

#    include "runtime.h"
#    include <dirent.h>

#    include <unistd.h>

#    include <link.h>

namespace Engine {
namespace Dl {

    struct SharedLibraryQueryState {
        dirent *mData;
    };

    static bool skipSymbolic(void *handle, SharedLibraryQueryState &data)
    {
        while (strcmp(data.mData->d_name, ".") == 0 || strcmp(data.mData->d_name, "..") == 0) {
            data.mData = readdir((DIR *)handle);
            if (!data.mData)
                return false;
        }

        return true;
    }

    SharedLibraryIterator::SharedLibraryIterator(const SharedLibraryQuery *query)
        : mQuery(query)
        , mBuffer { new SharedLibraryQueryState, [](SharedLibraryQueryState *s) { delete s; } }
        , mHandle(opendir(query->path().c_str()))
    {
        if (mHandle) {
            ++(*this);
        }
    }

    void SharedLibraryIterator::close()
    {
        if (mHandle) {
            closedir((DIR *)mHandle);
            mHandle = nullptr;
        }
    }

    void SharedLibraryIterator::operator++()
    {
        mBuffer->mData = readdir((DIR *)mHandle);
        if (!mBuffer->mData || !skipSymbolic(mHandle, *mBuffer))
            close();
    }

    const char *filename(SharedLibraryQueryState &data)
    {
        return data.mData->d_name;
    }

    SharedLibraryQuery listSharedLibraries()
    {
#    if LINUX
        char buffer[512];

        auto result = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        assert(result > 0);
        buffer[result] = '\0';

        return SharedLibraryQuery { Filesystem::Path { buffer }.parentPath() };
#    elif ANDROID
        return SharedLibraryQuery { Filesystem::Path { "/data/data/com.Madgine.MadgineLauncher/lib" } }; //TODO
#    elif EMSCRIPTEN
        std::terminate();
#    else
#        error "Unsupported Platform!"
#    endif
    }

    /*static int visitModule(struct dl_phdr_info *info, size_t size, void *data) {
			if (info->dlpi_name)
			{
				Filesystem::Path file = Filesystem::Path(info->dlpi_name).filename();
				if (file.extension() == SHARED_LIB_SUFFIX)
				{
					std::string name = file.stem();
					if (StringUtil::startsWith(name, SHARED_LIB_PREFIX))
					{
						std::set<std::string> *modules = static_cast<std::set<std::string> *>(data);
						modules->insert(name.substr(strlen(SHARED_LIB_PREFIX)));
					}
				}
			}
			return 0;
		}

		std::set<std::string> listLoadedLibraries()
		{
			std::set<std::string> result;
			dl_iterate_phdr(visitModule, &result);
			return result;
		}*/

}
}

#endif