#include "../interfaceslib.h"

#include "memory.h"

#ifdef _WIN32
#include <Windows.h>

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
#ifdef _WIN64
        /* These items are reversed on Win64 to eliminate gaps in the struct
         * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
         * maintained in the debug heap.
         */
        int                         nBlockUse;
        size_t                      nDataSize;
#else  /* _WIN64 */
        size_t                      nDataSize;
        int                         nBlockUse;
#endif  /* _WIN64 */
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

#endif

#include <iostream>

namespace Engine {
	namespace Core {

		thread_local size_t MemoryTracker::ThreadLocal::sHiddenStackEntries = 0;

		MemoryTracker MemoryTracker::sInstance;

		MemoryTracker *&MemoryTracker::getSingleton()
		{
			static MemoryTracker *ptr = nullptr;
			return ptr;
		}

		void * MemoryTracker::allocate(size_t s)
		{
			++ThreadLocal::sHiddenStackEntries;

			return original_new(s);	
		}

		void MemoryTracker::deallocate(void * p)
		{	
			original_delete(p);
		}

		size_t MemoryTracker::overhead()
		{
			return mSafeMemUsage;
		}

		size_t MemoryTracker::totalMemory()
		{
			return mTotalMemory;
		}

#ifdef _WIN32
		const std::unordered_map<MemoryTracker::StackTrace, TracedAllocationData, std::hash<MemoryTracker::StackTrace>, std::equal_to<MemoryTracker::StackTrace>, MemoryTracker::UntrackedAllocator<std::pair<const MemoryTracker::StackTrace, TracedAllocationData>>>& MemoryTracker::stacktraces()
		{
			return mStacktraces;
		}

		int MemoryTracker::win32Hook(int allocType, void * userData, size_t size, int blockType, long requestNumber, const unsigned char * filename, int lineNumber)
		{
			switch (allocType) {
			case _HOOK_ALLOC:
				sInstance.onMalloc(requestNumber, size);
				break;
			case _HOOK_FREE:
				_CrtMemBlockHeader *header = (((_CrtMemBlockHeader*)userData)-1);			
				sInstance.onFree(header->lRequest, header->nDataSize);
				break;
			}

			return true;
		}
#endif

		
		MemoryTracker::MemoryTracker() :
#ifdef _WIN32
			mOldHook(_CrtSetAllocHook(&MemoryTracker::win32Hook)),
#endif
			mTotalMemory(0),
			mSafeMemUsage(0),
			mUnknownAllocationSize(0),
			mStacktraces(*this),
			mAllocations(*this),
			mUnknownAllocations(*this)
		{			
		}

		MemoryTracker::~MemoryTracker() {
#ifdef _WIN32
			_CrtSetAllocHook(mOldHook);
#endif

			std::cout << "-------- Madgine Memory Tracker Report --------" << std::endl;
			
			for (auto &track : mStacktraces){
				if (track.second.mSize == 0)
					continue;
				for (Util::TraceBack &trace : track.first){
					std::cout << trace << std::endl;
				}
				std::cout << "Current Size: " << track.second.mSize << std::endl;
			}

			std::cout << "-------- Unknown Deallocations --------" << std::endl;
 
			for (auto &track : mUnknownAllocations){
				if (track.second.mSize == 0)
					continue;
				for (Util::TraceBack &trace : track.first){
					std::cout << trace << std::endl;
				}
				std::cout << "Total Size: " << track.second.mSize << std::endl;
			}

			std::cout << "-------- Madgine Memory Tracker Summary --------" << std::endl;

			std::cout << "Total Leak: " << mTotalMemory << std::endl;
			std::cout << "Total Overhead: " << mSafeMemUsage << std::endl;

			std::cout << "-------- Madgine Memory Tracker End --------" << std::endl;
		}

		void * MemoryTracker::original_new(size_t s)
		{
			++ThreadLocal::sHiddenStackEntries;
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
			mSafeMemUsage += s;
			
			static HANDLE heap = GetProcessHeap();
			return HeapAlloc(heap, HEAP_GENERATE_EXCEPTIONS, s);

#else
#error "Unsupported Platform!"
#endif
		}

		void MemoryTracker::safe_dealloc(void * p)
		{
#ifdef _WIN32
			
			static HANDLE heap = GetProcessHeap();
			assert(HeapFree(heap, 0, p));

#else
#error "Unsupported Platform!"
#endif
		}

		void MemoryTracker::onMalloc(uintptr_t id, size_t s)
		{
			mTotalMemory += s;

			auto pib = mStacktraces.try_emplace(StackTrace::getCurrent(6 + ThreadLocal::sHiddenStackEntries));

			ThreadLocal::sHiddenStackEntries = 0;

			pib.first->second.mSize += s;

			auto pib2 = mAllocations.try_emplace(id);
			assert(pib2.second);
			pib2.first->second.mTrace = &pib.first->second;

		}

		void MemoryTracker::onFree(uintptr_t id, size_t s)
		{
			auto it = mAllocations.find(id);
			if (it != mAllocations.end()){			
				
				mTotalMemory -= s;

				
				assert(it != mAllocations.end());
				
				it->second.mTrace->mSize -= s;

				mAllocations.erase(it);
			}else{
				mUnknownAllocationSize += s;
				auto pib = mUnknownAllocations.try_emplace(StackTrace::getCurrent(6 + ThreadLocal::sHiddenStackEntries));
				pib.first->second.mSize += s;
			}
			ThreadLocal::sHiddenStackEntries = 0;
		}

	}
}