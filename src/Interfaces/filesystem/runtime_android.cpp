#include "../interfaceslib.h"

#if ANDROID

#include "runtime.h"
#include "runtime_android.h"

#include <android/asset_manager.h> 

namespace Engine {
	namespace Filesystem {
		static AAssetManager *sAssetManager = nullptr;

		void setAndroidAssetManager(AAssetManager * assetManager)
		{
			sAssetManager = assetManager;
		}

/*
		struct SharedLibraryQueryState {			
			const char *mData;
		};

		SharedLibraryIterator::SharedLibraryIterator(const SharedLibraryQuery * query) :
			mQuery(query),
			mBuffer(createSharedLibraryQueryState(), destroySharedLibraryQueryState),
			mHandle(AAssetManager_openDir(sAssetManager, query->path().c_str()))
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
				AAssetDir_close((AAssetDir*)mHandle);
				mHandle = nullptr;
			}
		}

		void SharedLibraryIterator::operator++()
		{
			mBuffer->mData = AAssetDir_getNextFileName((AAssetDir*)mHandle);
			if (!mBuffer->mData)
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
			return data.mData;
		}


		SharedLibraryQuery listSharedLibraries()
		{
#ifdef __i386__
#define archDir "x86"
#else
#error "Unknown Architecture!"
#endif
			return SharedLibraryQuery{ Path("lib/" archDir) };
#undef archDir
		}
*/
	}
}

#endif