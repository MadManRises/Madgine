#include "../../interfaceslib.h"

#include "memory.h"

#include "untrackedmemoryresource.h"
#include "statsmemoryresource.h"

#ifdef _WIN32
#define NOMINMAX
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

#else

#include <iostream>

#endif

namespace Engine {
	namespace Debug {
		namespace Memory {

			MemoryTracker *MemoryTracker::sSingleton = nullptr;

			MemoryTracker &MemoryTracker::getSingleton()
			{
				assert(sSingleton);
				return *sSingleton;
			}

			size_t MemoryTracker::overhead()
			{
				return UntrackedMemoryResource::sInstance()->allocation_size();
			}

			size_t MemoryTracker::totalMemory()
			{
				return mTotalMemory;
			}

#ifdef _WIN32
			const std::pmr::unordered_map<FullStackTrace, TracedAllocationData>& MemoryTracker::stacktraces()
			{
				return mFullStacktraces;
			}

			std::atomic<const std::pair<const FullStackTrace, TracedAllocationData> *>& MemoryTracker::linkedFront()
			{
				return mLinkedFront;
			}

			int MemoryTracker::win32Hook(int allocType, void * userData, size_t size, int blockType, long requestNumber, const unsigned char * filename, int lineNumber)
			{
				_CrtMemBlockHeader *header;
				switch (allocType) {
				case _HOOK_ALLOC:
					sSingleton->onMalloc(requestNumber, size);
					break;
				case _HOOK_FREE:
					header = ((_CrtMemBlockHeader*)userData) - 1;
					sSingleton->onFree(header->lRequest, header->nDataSize);
					break;
				case _HOOK_REALLOC:
					header = ((_CrtMemBlockHeader*)userData) - 1;
					sSingleton->onFree(header->lRequest, header->nDataSize);
					sSingleton->onMalloc(requestNumber, size);
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
				mUnknownAllocationSize(0),				
				mStacktraces(UntrackedMemoryResource::sInstance()),
				mUnknownAllocations(UntrackedMemoryResource::sInstance()),
				mUnknownFullStacktraces(UntrackedMemoryResource::sInstance()),
				mFullStacktraces(UntrackedMemoryResource::sInstance()),
				mAllocations(UntrackedMemoryResource::sInstance()),
				mLinkedFront(nullptr)
			{
				assert(!sSingleton);
				sSingleton = this;
			}

			MemoryTracker::~MemoryTracker() {
#ifdef _WIN32
				_CrtSetAllocHook(mOldHook);
#endif

				sSingleton = nullptr;

#ifndef _WIN32
#	define OutputDebugString(msg) std::cout << msg
#endif

				OutputDebugString("-------- Madgine Memory Tracker Report --------\n");

				for (auto &track : mFullStacktraces) {
					if (track.second.mSize == 0)
						continue;
					for (const TraceBack &trace : track.first) {
						OutputDebugString((std::to_string(trace) + '\n').c_str());
					}
					OutputDebugString(("Current Size: "s + std::to_string(track.second.mSize) + '\n').c_str());
				}

				OutputDebugString("-------- Unknown Deallocations --------\n");

				for (auto &track : mUnknownFullStacktraces) {
					if (track.second.mSize == 0)
						continue;
					for (const TraceBack &trace : track.first) {
						OutputDebugString((std::to_string(trace) + '\n').c_str());
					}
					OutputDebugString(("Total Size: "s + std::to_string(track.second.mSize) + '\n').c_str());
				}

				OutputDebugString("-------- Madgine Memory Tracker Summary --------\n");

				OutputDebugString(("Total Leak: "s + std::to_string(mTotalMemory) + '\n').c_str());
				OutputDebugString(("Total Overhead: "s + std::to_string(overhead()) + '\n').c_str());

				OutputDebugString("-------- Madgine Memory Tracker End --------\n");
			}

			void MemoryTracker::onMalloc(uintptr_t id, size_t s)
			{
				mTotalMemory += s;

				auto pib = mStacktraces.try_emplace(StackTrace::getCurrent(6));

				if (pib.second) {
					auto pib2 = mFullStacktraces.try_emplace(pib.first->first.calculateReadable());
					pib.first->second = &pib2.first->second;
					if (pib2.second) {
						pib2.first->second.mNext = mLinkedFront;
						mLinkedFront = &*pib2.first;
					}
				}
				

				pib.first->second->mSize += s;
				pib.first->second->mGeneration = std::min(pib.first->second->mGeneration, size_t(1));

				auto pib2 = mAllocations.try_emplace(id, pib.first->second);
				assert(pib2.second);

			}

			void MemoryTracker::onFree(uintptr_t id, size_t s)
			{
				auto it = mAllocations.find(id);
				if (it != mAllocations.end()) {

					mTotalMemory -= s;


					assert(it != mAllocations.end());

					it->second->mSize -= s;
					it->second->mGeneration = std::min(it->second->mGeneration, size_t(1));

					mAllocations.erase(it);
				}
				else {
					mUnknownAllocationSize += s;
					auto pib = mUnknownAllocations.try_emplace(StackTrace::getCurrent(6));
					if (pib.second) {
						auto pib2 = mUnknownFullStacktraces.try_emplace(pib.first->first.calculateReadable());
						pib.first->second = &pib2.first->second;
					}
					pib.first->second->mSize += s;
				}
			}

		}
	}
}