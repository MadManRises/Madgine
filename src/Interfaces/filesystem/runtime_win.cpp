#include "../interfaceslib.h"

#if WINDOWS

#include "runtime.h"

#include "../generic/templates.h"

#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>

namespace Engine 
{
	namespace Filesystem
	{

		struct SharedLibraryQueryState {
			WIN32_FIND_DATA mData;
		};

		static bool skipSymbolic(void *handle, SharedLibraryQueryState &data)
		{
			while (strcmp(data.mData.cFileName, ".") == 0 ||
				strcmp(data.mData.cFileName, "..") == 0)
				if (!FindNextFile(handle, &data.mData))
					return false;
			return true;
		}

		SharedLibraryIterator::SharedLibraryIterator(const SharedLibraryQuery * query) :
			mQuery(query),
			mBuffer(createSharedLibraryQueryState(), destroySharedLibraryQueryState),
			mHandle(FindFirstFile((query->path().str() + "/*").c_str(), &mBuffer->mData))
		{
			if (mHandle == INVALID_HANDLE_VALUE)
				mHandle = nullptr;
			else if (!skipSymbolic(mHandle, *mBuffer))			
				close();			
		}

		void SharedLibraryIterator::close()
		{
			if (mHandle)
			{
				FindClose(mHandle);
				mHandle = nullptr;
			}
		}

		void SharedLibraryIterator::operator++()
		{
			if (!FindNextFile(mHandle, &mBuffer->mData))
				close();
		}

		SharedLibraryQueryState * createSharedLibraryQueryState()
		{
			return new SharedLibraryQueryState;
		}

		void destroySharedLibraryQueryState(SharedLibraryQueryState * state)
		{
			delete state;
		}

		const char * filename(SharedLibraryQueryState & data)
		{
			return data.mData.cFileName;
		}


		SharedLibraryQuery listSharedLibraries()
		{
			char buffer[512];

			auto result = GetModuleFileName(nullptr, buffer, sizeof(buffer));
			assert(result > 0);			

			return SharedLibraryQuery{ Path(buffer).parentPath() };
		}

		std::set<std::string> listLoadedLibraries()
		{
			std::set<std::string> result;

			DWORD count;
			HMODULE modules[512];
			auto check = EnumProcessModules(GetCurrentProcess(), modules, static_cast<DWORD>(array_size(modules)), &count);
			assert(check);
			count /= sizeof(HMODULE);
			assert(count < array_size(modules));

			for (DWORD i = 0; i < count; ++i)
			{
				char buffer[512];
				auto check2 = GetModuleFileName(modules[i], buffer, sizeof(buffer));
				assert(check2);
				Filesystem::Path path = buffer;
				result.insert(path.stem());
			}

			return result;
		}

	}
}

#endif