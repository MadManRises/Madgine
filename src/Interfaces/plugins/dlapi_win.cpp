#include "../interfaceslib.h"

#if WINDOWS

#include "dlapi.h"
#include "../signalslot/taskguard.h"

#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>


namespace Engine {
namespace Plugins {

    void *openDll(const std::string &name)
    {
        UINT errorMode;
        SignalSlot::TaskGuard guard([&]() { errorMode = GetErrorMode(); }, [&]() { SetErrorMode(errorMode); });

        void *handle;
        if (name.empty())
            handle = GetModuleHandle(nullptr);
        else
            handle = LoadLibrary(name.c_str());
        SymRefreshModuleList(GetCurrentProcess());

        return handle;
    }

    void closeDll(void *handle)
    {
        auto result = FreeLibrary((HINSTANCE)handle);
        assert(result != 0);
    }

    const void *getDllSymbol(void *dllHandle, const std::string &symbolName)
    {
        return GetProcAddress((HINSTANCE)dllHandle, symbolName.c_str());
    }

    Filesystem::Path getDllFullPath(void *dllHandle, const std::string &symbolName)
    {
        char buffer[512];
        auto result = GetModuleFileName((HMODULE)dllHandle, buffer, sizeof(buffer));
        assert(result);
        return buffer;
    }
}
}

#endif