#include "../interfaceslib.h"

#if WINDOWS || !POSIX

#include "dlapi.h"

#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>

namespace Engine {
namespace Dl {

    static DWORD sLastError;

    DlAPIResult toResult(DWORD error, const char *op)
    {
        switch (error) {
        case ERROR_MOD_NOT_FOUND:
            return DlAPIResult::DEPENDENCY_ERROR;
        default:
            fprintf(stderr, "Unknown Windows Dl-Error-Code from %s: %lu\n", op, error);
            fflush(stderr);
            return DlAPIResult::UNKNOWN_ERROR;
        }
    }

    DlHandle openDll(const std::string &name)
    {
#if WINAPI_FAMILY_PARTITION(NONGAMESPARTITIONS)
        static struct Guard {
            Guard()
            {
                errorMode = GetErrorMode();
            }
            ~Guard()
            {
                SetErrorMode(errorMode);
            }
            UINT errorMode;
        } guard;
        SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

        void *handle;
        if (name.empty())
            handle = GetModuleHandle(nullptr);
        else
            handle = LoadLibrary(name.c_str());
        sLastError = GetLastError();
        SymRefreshModuleList(GetCurrentProcess());

        return DlHandle{ handle };
    }

    void closeDll(void *handle)
    {
        auto result = FreeLibrary((HINSTANCE)handle);
        sLastError = GetLastError();
        assert(result != 0);
    }

    const void *getDllSymbol(const DlHandle &dllHandle, const std::string &symbolName)
    {
        return reinterpret_cast<const void*>(GetProcAddress((HINSTANCE)dllHandle.get(), symbolName.c_str()));
    }

    Filesystem::Path getDllFullPath(const DlHandle &dllHandle, const std::string &symbolName)
    {
        char buffer[512];
        auto result = GetModuleFileName((HMODULE)dllHandle.get(), buffer, sizeof(buffer));
        assert(result);
        return buffer;
    }

    DlAPIResult getError(const char *op) {
        return toResult(sLastError, op);
    }
}
}

#endif
