#include "../interfaceslib.h"

#include "stacktrace.h"

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#elif __linux__
#include <execinfo.h>
#endif

namespace Engine{
    namespace Util{

        void getStackTrace(size_t skip, void **buffer, size_t size){
            size_t trace;
#ifdef _WIN32
            trace = CaptureStackBackTrace(skip + 1, size, buffer, NULL);
#elif __linux__
            std::unique_ptr<void*[]> tmpBuffer = std::make_unique<void*[]>(size + skip + 1);
            trace = backtrace(tmpBuffer.get, size + skip + 1);
            if (!memcpy_s(buffer, tmpBuffer.get() + skip + 1, size))
                throw 0;
#else
#error "Unsupported Platform!"
#endif
            if (trace < size)
                buffer[trace] = nullptr;
        }

        void resolveSymbols(void *const *data, std::unique_ptr<TraceBack> *buffer, size_t size){                
            constexpr size_t BUFFERSIZE = 512;
#ifdef _WIN32
            char infoBuffer[sizeof(SYMBOL_INFO) + BUFFERSIZE];
            PSYMBOL_INFO info = reinterpret_cast<PSYMBOL_INFO>(infoBuffer);
            info->MaxNameLen = BUFFERSIZE;
            info->SizeOfStruct = sizeof(SYMBOL_INFO);
            IMAGEHLP_LINE line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
            DWORD disp;
            static HANDLE process = GetCurrentProcess();
            static struct Guard {
                Guard(){
                    assert(SymSetOptions(SYMOPT_LOAD_LINES));    
                    assert(SymInitialize(process, NULL, TRUE));
                }
                ~Guard(){
                    assert(SymCleanup(process));
                }
            } guard = {};
#elif __linux__
            char **symbols = backtrace_symbols(data, size);
#endif
            
            for (int i = 0; i < size; ++i){
                if (!buffer[i]){
#ifdef _WIN32
                    if (SymFromAddr(process, reinterpret_cast<DWORD64>(data[i]), nullptr, info)){
                        if (SymGetLineFromAddr(process, reinterpret_cast<DWORD64>(data[i]), &disp, &line)){
                            buffer[i] = std::make_unique<TraceBack>(info->Name, line.FileName, line.LineNumber);
                        }else{
                            buffer[i] = std::make_unique<TraceBack>(info->Name);
                        }
                    }
#elif __linux__
                    if (symbols && symbols[i]){
                        buffer[i] = std::make_unique<TraceBack>(symbols[i]);
                    }
#else
#error "Unsupported Platform!"
#endif
                    if (!buffer[i]){
                        buffer[i] = std::make_unique<TraceBack>();
                    }
                }
            }
#ifdef __linux__
            if (symbols)
                free(symbols);
#endif
        }
    }
}