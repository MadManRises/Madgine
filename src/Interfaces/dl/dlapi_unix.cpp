#include "../interfaceslib.h"

#if UNIX

#include "dlapi.h"

#include <dlfcn.h>
#include <unistd.h>

#include "../util/exception.h"

namespace Engine {
namespace Dl {

    static int sLastError;

    DlAPIResult toResult(int error, const char *op)
    {
        switch (error) {
        default:
            fprintf(stderr, "Unknown Unix Dl-Error-Code from %s: %lu\n", op, error);
            fflush(stderr);
            return DlAPIResult::UNKNOWN_ERROR;
        }
    }

    void *openDll(const std::string &name)
    {
        void *handle;

        if (name.empty())
            handle = dlopen(nullptr, RTLD_LAZY);
        else
            handle = dlopen(name.c_str(), RTLD_NOW);
        sLastError = errno;

        return handle;
    }

    void closeDll(void *handle)
    {
        auto result = dlclose(handle);
        assert(result == 0);
    }

    const void *getDllSymbol(void *dllHandle, const std::string &symbolName)
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

    DlAPIResult getError(const char *op) {
        return toResult(sLastError, op);
    }
}
}

#endif