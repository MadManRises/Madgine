#include "../interfaceslib.h"

#include "memory.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Engine {
	namespace Core {

		thread_local bool MemoryTracker::ThreadLocalStorage::sRequesting = false;
		thread_local bool MemoryTracker::ThreadLocalStorage::sTryLock = false;

		MemoryTracker &MemoryTracker::sInitializer = sInstance();

		MemoryTracker &MemoryTracker::sInstance()
		{
			static MemoryTracker mgr;
			return mgr;
		}

		void * MemoryTracker::allocate(size_t s)
		{

			ThreadLocalStorage::sRequesting = true;
			void *p = original_new(s);	
			auto pIb = mAllocations.try_emplace(p, s);
			assert(pIb.second);
			mMutex.unlock();

			mRegisteredMemory += s;

			return p;
		}

		void MemoryTracker::deallocate(void * p)
		{
			original_delete(p);			
		}

		size_t MemoryTracker::overhead()
		{
			return mVirtualMemUsage;
		}

		size_t MemoryTracker::rawMemory()
		{
			return mRawMemory;
		}

		size_t MemoryTracker::registeredMemory()
		{
			return mRegisteredMemory;
		}

#ifdef _WIN32
		const std::unordered_map<unsigned long, MemoryTracker::BackTrace, std::hash<unsigned long>, std::equal_to<unsigned long>, MemoryTracker::UntrackedAllocator<std::pair<const unsigned long, MemoryTracker::BackTrace>>>& MemoryTracker::backtraces()
		{
			return mBacktraces;
		}
		int MemoryTracker::win32Hook(int allocType, void * userData, size_t size, int blockType, long requestNumber, const unsigned char * filename, int lineNumber)
		{
			switch (allocType) {
			case _HOOK_ALLOC:
				sInstance().onMalloc(size);
				break;
			case _HOOK_FREE:
				sInstance().onFree(userData, size);
				break;
			}

			return true;
		}
#endif

		
		MemoryTracker::MemoryTracker() :
			mRawMemory(0),
			mRegisteredMemory(0),
			mPageBase(nullptr),
			mVirtualMemUsage(0),
			mAllocations(*this),
			mBacktraces(*this)
		{
#ifdef _WIN32
			mOldHook = _CrtSetAllocHook(&MemoryTracker::win32Hook);
#endif
			
		}

		MemoryTracker::~MemoryTracker() {
#ifdef _WIN32
			_CrtSetAllocHook(mOldHook);
#endif
		}

		void * MemoryTracker::original_new(size_t s)
		{
			void *p = malloc(s);
			while (!p) {
				auto new_handler = std::get_new_handler();
				if (!new_handler)
					throw std::bad_alloc();
				p = malloc(s);
			}
			return p;
		}

		void MemoryTracker::original_delete(void * p)
		{
			if (p)
				free(p);
		}

		void * MemoryTracker::safe_alloc(size_t s)
		{
#ifdef _WIN32
			
			static auto pageSize = [](){
				SYSTEM_INFO info;
				GetSystemInfo(&info);
				return info;
			}().dwPageSize;

			if (!mPageBase || reinterpret_cast<uintptr_t>(mPageBase) % pageSize + s > pageSize)
			{
				mPageBase = static_cast<char*>(VirtualAlloc(NULL, s, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
			}

			void *p = mPageBase;

			mPageBase += s;
			mVirtualMemUsage += s;

			if (reinterpret_cast<uintptr_t>(mPageBase) % pageSize == 0)
				mPageBase = nullptr;

			return p;
#endif
		}

		void MemoryTracker::safe_dealloc(void * p)
		{
#ifdef _WIN32
			//DO NOTHING!!!
#endif
		}

		void MemoryTracker::onMalloc(size_t s)
		{
			mRawMemory += s;
			if (!ThreadLocalStorage::sTryLock) 
			{
				ThreadLocalStorage::sTryLock = true;
				mMutex.lock();
				ThreadLocalStorage::sTryLock = false;

				constexpr size_t BUFFER_SIZE = 3;

				void *buffer[BUFFER_SIZE];

				DWORD hash;

				auto frames = CaptureStackBackTrace(6, BUFFER_SIZE, buffer, &hash);

				mBacktraces.try_emplace(hash, std::vector<void*, UntrackedAllocator<void*>>{buffer, buffer + frames, *this });

				if (ThreadLocalStorage::sRequesting)
				{
					ThreadLocalStorage::sRequesting = false;
				}
				else 
				{
					mMutex.unlock();
				}
			}
		}

		void MemoryTracker::onFree(void * p, size_t s)
		{
			mRawMemory -= s;
			if (!ThreadLocalStorage::sTryLock) 
			{
				ThreadLocalStorage::sTryLock = true;
				std::scoped_lock<std::mutex> lock(mMutex);
				ThreadLocalStorage::sTryLock = false;
				auto it = mAllocations.find(p);
				if (it != mAllocations.end()) 
				{
					mRegisteredMemory -= it->second.mSize;
					mAllocations.erase(it);
				}
			}
		}

	}
}