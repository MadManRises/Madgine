#include "../interfaceslib.h"

#if UNIX

#include "../signalslot/taskguard.h"
#include "dlapi.h"

#include <dlfcn.h>
#include <unistd.h>

namespace Engine {
namespace Plugins {

    void *openDll(const std::string &name)
    {
        void *handle;

        if (name.empty())
            handle = dlopen(nullptr, RTLD_LAZY);
        else
            handle = dlopen(name.c_str(), RTLD_NOW);
        if (!handle)
            throw std::exception(dlerror());

        return handle;
    }

    void closeDll(void *handle)
    {
        auto result = dlclose(handle);
        assert(result == 0);
    }

    void *getDllSymbol(void *dllHandle, const std::string &symbolName)
    {
        return dlsym(dllHandle, symbolName.c_str());
    }

    Filesystem::Path getDllFullPath(void *dllHandle, const std::string &symbolName)
    {
        Dl_info info;
        auto result = dladdr(getDllSymbol(dllHandle, symbolName.c_str()), &info);
        assert(result);
        return info.dli_fname;
    }
}
}

#endif