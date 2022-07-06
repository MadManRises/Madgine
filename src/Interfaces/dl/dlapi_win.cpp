#include "../interfaceslib.h"

#if WINDOWS || !POSIX

#include "dlapi.h"

#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>

namespace Engine {
namespace Dl {

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

    DlHandle::~DlHandle(){
        close();
    }

    DlAPIResult DlHandle::open(std::string_view name)
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
        
        if (name.empty())
            mHandle = GetModuleHandle(nullptr);
        else
            mHandle = LoadLibrary(name.data());

        if (!mHandle){
            DWORD error = GetLastError();
            return toResult(error, "DlHandle::open");
        }
        
        SymRefreshModuleList(GetCurrentProcess());

        return DlAPIResult::SUCCESS;
    }

    DlAPIResult DlHandle::close()
    {
        if (mHandle){
            auto result = FreeLibrary((HINSTANCE)mHandle);

            if (result != 0)
                return toResult(GetLastError(), "DlHandle::close");
            
            mHandle = nullptr;
        }
        return DlAPIResult::SUCCESS;
    }

    const void *DlHandle::getSymbol(std::string_view name) const
    {
        return reinterpret_cast<const void*>(GetProcAddress((HINSTANCE)mHandle, name.data()));
    }

    Filesystem::Path DlHandle::fullPath(std::string_view symbolName) const
    {
        char buffer[512];
        auto result = GetModuleFileName((HMODULE)mHandle, buffer, sizeof(buffer));
        assert(result);
        return buffer;
    }

}
}

#endif
