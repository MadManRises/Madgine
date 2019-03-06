#include "../interfaceslib.h"

#if UNIX

#include "runtime.h"
#include <dirent.h>

#include <unistd.h>

namespace Engine
{
	namespace Filesystem
	{

		struct SharedLibraryQueryState {
			dirent *mData;
		};

		static bool skipSymbolic(void *handle, SharedLibraryQueryState &data)
		{
			while (strcmp(data.mData->d_name, ".") == 0 ||
				strcmp(data.mData->d_name, "..") == 0)
			{
				data.mData = readdir((DIR*)handle);
				if (!data.mData)
					return false;
			}

			return true;
		}

		SharedLibraryIterator::SharedLibraryIterator(const SharedLibraryQuery * query) :
			mQuery(query),
			mBuffer(createSharedLibraryQueryState(), destroySharedLibraryQueryState),
			mHandle(opendir(query->path().c_str()))
		{
			if (mHandle)
			{
				++(*this);
			}
		}

		void SharedLibraryIterator::close()
		{
			if (mHandle)
			{
				closedir((DIR*)mHandle);
				mHandle = nullptr;
			}
		}

		void SharedLibraryIterator::operator++()
		{
			mBuffer->mData = readdir((DIR*)mHandle);			
			if (!mBuffer->mData || !skipSymbolic(mHandle, *mBuffer))
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
			return data.mData->d_name;
		}


		SharedLibraryQuery listSharedLibraries()
		{
#if LINUX
			char buffer[512];

			auto result = readlink("/proc/self/exe", buffer, sizeof(buffer));
			assert(result > 0);

			return SharedLibraryQuery{ Path(buffer).parentPath() };
#else
			return SharedLibraryQuery{ Path("/data/data/com.Madgine.MadgineLauncher/lib") };
#endif
		}

	}
}

#endif