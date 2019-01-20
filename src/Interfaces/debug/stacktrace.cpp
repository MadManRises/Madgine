#include "../interfaceslib.h"

#include "stacktrace.h"
#include "memory/untrackedmemoryresource.h"
#include "memory/statsmemoryresource.h"

#if WINDOWS
#include <Windows.h>
#include <DbgHelp.h>
#elif LINUX
#include <execinfo.h>
#endif

namespace Engine {
	namespace Debug {

		static void findAndReplaceAll(std::pmr::string & data, const char *toSearch, const char *replaceStr)
		{
			// Get the first occurrence
			size_t pos = data.find(toSearch);

			size_t toSearchSize = strlen(toSearch);
			size_t replaceSize = strlen(replaceStr);

			// Repeat till end is reached
			while (pos != std::string::npos)
			{
				// Replace this occurrence of Sub String
				data.replace(pos, toSearchSize, replaceStr);
				// Get the next occurrence from the current position
				pos = data.find(toSearch, pos + replaceSize);
			}
		}

		void getStackTrace(size_t skip, void **buffer, size_t size) {
			size_t trace;
#if WINDOWS
			trace = CaptureStackBackTrace((DWORD)skip + 1, (DWORD)size, buffer, NULL);
#elif ANDROID
			trace = 0;
#elif LINUX
			void *tmpBuffer[128];
			assert(size + skip + 1 < sizeof(tmpBuffer) / sizeof(tmpBuffer[0]));			
			trace = backtrace(tmpBuffer, size + skip + 1);
			memcpy(buffer, tmpBuffer + skip + 1, size * sizeof(void*));				
#else
#error "Unsupported Platform!"
#endif
			if (trace < size)
				buffer[trace] = nullptr;
		}

		FullStackTrace resolveSymbols(void *const *data, size_t size) {
			std::pmr::memory_resource *resource = Memory::UntrackedMemoryResource::sInstance();
			FullStackTrace result(resource);

			constexpr size_t BUFFERSIZE = 1024;
#if WINDOWS
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
				Guard() {
					auto result = SymSetOptions(SYMOPT_LOAD_LINES) && SymInitialize(GetCurrentProcess(), NULL, TRUE);
					if (!result) {
						int error = GetLastError();
						throw 0;
					}					
				}
				~Guard() {
					auto result = SymCleanup(process);
					assert(result);
				}
			} guard = {};
			struct BufferObject {
				BufferObject(std::pmr::memory_resource *resource, const char *function, const char *file = "", int lineNr = -1) :
					mFunction(function, resource), mFile(file, resource), mLineNr(lineNr) {}
				std::pmr::string mFunction, mFile;
				int mLineNr;
			};
			using AddressBuffer = std::pmr::unordered_map<void *, std::optional<BufferObject>>;
			static AddressBuffer addressBuffer{ resource };
#elif ANDROID
			size = 0;
#elif LINUX
			char **symbols = backtrace_symbols(data, size);
#endif

			for (int i = 0; i < size && result.size() < 6; ++i) {
#if WINDOWS
				auto pib = addressBuffer.try_emplace(data[i]);
				if (pib.second) {
					if (SymFromAddr(process, reinterpret_cast<DWORD64>(data[i]), nullptr, info)) {
						if (strncmp(newSymbol, info->Name, sizeof(newSymbol) - 1) == 0)
							continue;
						if (strncmp(stdSymbol, info->Name, sizeof(stdSymbol) - 1) == 0)
							pib.first->second = BufferObject{ resource, info->Name };
						else if (SymGetLineFromAddr(process, reinterpret_cast<DWORD64>(data[i]), &disp, &line))
							pib.first->second = BufferObject{ resource, info->Name, line.FileName, (int)line.LineNumber };
						else
							pib.first->second = BufferObject{ resource, info->Name };

						findAndReplaceAll(pib.first->second->mFunction, "basic_string<char,std::char_traits<char>,std::allocator<char> >", "string");
						
					}
					else {
						pib.first->second = BufferObject{ resource, "<Missing Data>" };
					}
				}
				if (pib.first->second) {
					if (!result.empty() && result.back().mLineNr == -1 && pib.first->second->mLineNr == -1)
						result.erase(--result.end());
					result.emplace_back(data[i], pib.first->second->mFunction.c_str(), pib.first->second->mFile.c_str(), pib.first->second->mLineNr);
				}
#elif ANDROID
#elif LINUX
				if (symbols && symbols[i]) {
					result.emplace_back(data[i], symbols[i]);
				}
#else
#error "Unsupported Platform!"
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