#pragma once

#include "../stacktrace.h"

namespace Engine {
	namespace Debug {
		namespace Memory {

			struct TracedAllocationData {
				size_t mSize = 0;
				mutable size_t mGeneration = 0;
				mutable const std::pair<const FullStackTrace, TracedAllocationData> *mNext = nullptr;
				mutable size_t mBlockSize = 0;
			};



			struct INTERFACES_EXPORT MemoryTracker {

				using StackTrace = StackTrace<64>;				

				size_t overhead();
				size_t totalMemory();
				
				const std::pmr::unordered_map<FullStackTrace, TracedAllocationData> &stacktraces();
				std::atomic<const std::pair<const FullStackTrace, TracedAllocationData> *>& linkedFront();

				static MemoryTracker &getSingleton();

				MemoryTracker();
				MemoryTracker(const MemoryTracker &) = delete;
				~MemoryTracker();

			private:

				//STATS
				void onMalloc(uintptr_t id, size_t s);
				void onFree(uintptr_t id, size_t s);

				size_t mTotalMemory;
				size_t mUnknownAllocationSize;

				std::atomic<const std::pair<const FullStackTrace, TracedAllocationData>*> mLinkedFront;

				std::pmr::unordered_map<FullStackTrace, TracedAllocationData> mUnknownFullStacktraces;
				std::pmr::unordered_map<StackTrace, TracedAllocationData*> mUnknownAllocations;
				std::pmr::unordered_map<uintptr_t, TracedAllocationData*> mAllocations;
				std::pmr::unordered_map<StackTrace, TracedAllocationData*> mStacktraces;
				std::pmr::unordered_map<FullStackTrace, TracedAllocationData> mFullStacktraces;
				
			private:

				//STATS IMPL
#ifdef _WIN32
				int(*mOldHook)(int, void*, size_t, int, long, const unsigned char *, int);

				static int win32Hook(int allocType, void *userData, size_t size, int
					blockType, long requestNumber, const unsigned char *filename, int
					lineNumber);

#endif

				static MemoryTracker *sSingleton;


				struct ThreadLocal {
				};

			};



		}
	}
}

