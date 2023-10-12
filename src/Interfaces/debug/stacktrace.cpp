#include "../interfaceslib.h"

#ifndef NDEBUG

#include "memory/statsmemoryresource.h"
#include "memory/untrackedmemoryresource.h"
#include "stacktrace.h"

#if WINDOWS
#    include <Windows.h>
#    include <DbgHelp.h>
#elif LINUX
#    include <execinfo.h>
#endif

namespace Engine {
namespace Debug {

    //TODO
    struct DefaultMemResource : std::pmr::memory_resource {
        virtual void *do_allocate(std::size_t bytes, std::size_t alignment) override
        {
            return malloc(bytes);
        }

        virtual void do_deallocate(void *ptr, std::size_t bytes, std::size_t alignment) override
        {
            free(ptr);
        }

        virtual bool do_is_equal(const std::pmr::memory_resource &res) const noexcept override
        {
            return this == &res;
        }
    };

    void getStackTrace(size_t skip, void **buffer, size_t size)
    {
        size_t trace;
#if WINDOWS
        trace = CaptureStackBackTrace((DWORD)skip + 1, (DWORD)size, buffer, NULL);
#elif ANDROID
        trace = 0;
#elif LINUX
        void *tmpBuffer[128];
        assert(size + skip + 1 < sizeof(tmpBuffer) / sizeof(tmpBuffer[0]));
        trace = backtrace(tmpBuffer, size + skip + 1);
        memcpy(buffer, tmpBuffer + skip + 1, size * sizeof(void *));
#elif EMSCRIPTEN
        trace = 0;
#elif OSX
        trace = 0;
#elif IOS
        trace = 0;
#else
#    error "Unsupported Platform!"
#endif
        if (trace < size)
            buffer[trace] = nullptr;
    }

    FullStackTrace resolveSymbols(void *const *data, size_t size)
    {
#if ENABLE_MEMTRACKING
        std::pmr::memory_resource *resource = Memory::UntrackedMemoryResource::sInstance();
#else
        static DefaultMemResource defMem;
        std::pmr::memory_resource *resource = &defMem;
#endif
        FullStackTrace result(resource);
        result.reserve(size);

#if WINDOWS
#    if WINAPI_FAMILY_PARTITION(NONGAMESPARTITIONS)
        constexpr size_t BUFFERSIZE = 1024;
        char infoBuffer[sizeof(SYMBOL_INFO) + BUFFERSIZE];
        PSYMBOL_INFO info = reinterpret_cast<PSYMBOL_INFO>(infoBuffer);
        info->MaxNameLen = BUFFERSIZE;
        info->SizeOfStruct = sizeof(SYMBOL_INFO);
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        DWORD disp;
        constexpr const char newSymbol[] = "operator new";
        constexpr const char stdSymbol[] = "std::";
        static HANDLE process = GetCurrentProcess();
        static struct Guard {
            Guard()
            {
                auto result = SymSetOptions(SYMOPT_LOAD_LINES) && SymInitialize(GetCurrentProcess(), NULL, TRUE);
                if (!result) {
                    int error = GetLastError();
                    (void)error;
                    std::terminate();
                }
            }
            ~Guard()
            {
                auto result = SymCleanup(process);
                assert(result);
            }
        } guard = {};
        struct BufferObject {
            BufferObject(std::pmr::memory_resource *resource, const char *function, const char *file = "", int lineNr = -1)
                : mFunction(function, resource)
                , mFile(file, resource)
                , mLineNr(lineNr)
            {
            }
            std::pmr::string mFunction, mFile;
            int mLineNr;
        };
        using AddressBuffer = std::pmr::unordered_map<void *, std::optional<BufferObject>>;
        static AddressBuffer addressBuffer { resource };
#    else
        size = 0;
#    endif
#elif ANDROID
        size = 0;
#elif LINUX
        char **symbols = backtrace_symbols(data, size);
#elif EMSCRIPTEN
        size = 0;
#endif

        for (int i = 0; i < size; ++i) {
#if WINDOWS
#    if WINAPI_FAMILY_PARTITION(NONGAMESPARTITIONS)
            auto pib = addressBuffer.try_emplace(data[i]);
            if (pib.second) {
                if (SymFromAddr(process, reinterpret_cast<DWORD64>(data[i]), nullptr, info)) {
                    if (strncmp(newSymbol, info->Name, sizeof(newSymbol) - 1) == 0)
                        continue;
                    if (strncmp(stdSymbol, info->Name, sizeof(stdSymbol) - 1) == 0)
                        pib.first->second = BufferObject { resource, info->Name };
                    else if (SymGetLineFromAddr(process, reinterpret_cast<DWORD64>(data[i]), &disp, &line))
                        pib.first->second = BufferObject { resource, info->Name, line.FileName, (int)line.LineNumber };
                    else
                        pib.first->second = BufferObject { resource, info->Name };

                    StringUtil::replace(pib.first->second->mFunction, "basic_string<char,std::char_traits<char>,std::allocator<char> >", "string");

                } else {
                    pib.first->second = BufferObject { resource, "<Missing Data>" };
                }
            }
            if (pib.first->second) {
                if (!result.empty() && result.back().mLineNr == -1 && pib.first->second->mLineNr == -1)
                    result.erase(--result.end());
                result.emplace_back(data[i], pib.first->second->mFunction.c_str(), pib.first->second->mFile.c_str(), pib.first->second->mLineNr);
            }
#    endif
#elif ANDROID
#elif LINUX
            if (symbols && symbols[i]) {
                result.emplace_back(data[i], symbols[i]);
            }
#elif EMSCRIPTEN
#elif OSX
#elif IOS
#else
#    error "Unsupported Platform!"
#endif
        }
#if LINUX
        if (symbols)
            free(symbols);
#endif
        return result;
    }

}
}

#endif
