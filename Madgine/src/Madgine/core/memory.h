#pragma once

#include "../util/stacktrace.h"

namespace Engine {
	namespace Core {

		struct TracedAllocationData{
			size_t mSize = 0;
		};

		struct AllocationData{
			TracedAllocationData *mTrace;
		};


		struct INTERFACES_EXPORT MemoryTracker {		

			static void *allocate(size_t s);
			static void deallocate(void *p);

			size_t overhead();
			size_t totalMemory();

		private:
			MemoryTracker();
			MemoryTracker(const MemoryTracker &) = delete;
			~MemoryTracker();

			//SAFE ALLOC
			size_t mSafeMemUsage;

			void *safe_alloc(size_t s);
			void safe_dealloc(void *p);

			template <class T>
			struct UntrackedAllocator {
				typedef T value_type;
				UntrackedAllocator(MemoryTracker &tracker) : mTracker(tracker) {};
				template <class U> constexpr UntrackedAllocator(const UntrackedAllocator<U>& other) noexcept : mTracker(other.mTracker) {}
				[[nodiscard]] value_type* allocate(std::size_t n) {
					return static_cast<value_type*>(mTracker.safe_alloc(n * sizeof(T)));
				}
				void deallocate(value_type* p, std::size_t) noexcept { mTracker.safe_dealloc(p); }

				MemoryTracker &mTracker;
			};
			template <class T, class U>
			friend bool operator==(const UntrackedAllocator<T>&, const UntrackedAllocator<U>&) { return true; }
			template <class T, class U>
			friend bool operator!=(const UntrackedAllocator<T>&, const UntrackedAllocator<U>&) { return false; }


			//STATS
			void onMalloc(uintptr_t id, size_t s);
			void onFree(uintptr_t id, size_t s);

			size_t mTotalMemory;
			size_t mUnknownAllocationSize;

			using StackTrace = Util::StackTrace<3>;

			std::unordered_map<StackTrace, TracedAllocationData, std::hash<StackTrace>, std::equal_to<StackTrace>, UntrackedAllocator<std::pair<const StackTrace, TracedAllocationData>>> mStacktraces;
			std::unordered_map<StackTrace, TracedAllocationData, std::hash<StackTrace>, std::equal_to<StackTrace>, UntrackedAllocator<std::pair<const StackTrace, TracedAllocationData>>> mUnknownAllocations;
			std::unordered_map<uintptr_t, AllocationData, std::hash<uintptr_t>, std::equal_to<uintptr_t>, UntrackedAllocator<std::pair<const uintptr_t, AllocationData>>> mAllocations;

		public:
			const std::unordered_map<StackTrace, TracedAllocationData, std::hash<StackTrace>, std::equal_to<StackTrace>, UntrackedAllocator<std::pair<const StackTrace, TracedAllocationData>>> &stacktraces();
		private:

			//STATS IMPL
#ifdef _WIN32
			int(*mOldHook)(int, void*, size_t, int, long, const unsigned char *, int);

			static int win32Hook(int allocType, void *userData, size_t size, int
				blockType, long requestNumber, const unsigned char *filename, int
				lineNumber);

#else
#error "Unsupported Platform!"
#endif

			//ORIGINAL
			static void *original_new(size_t s);
			static void original_delete(void *p);


			static MemoryTracker sInstance;
			static MemoryTracker *&getSingleton();

			struct ThreadLocal{
				static thread_local size_t sHiddenStackEntries;
			};

		};



	}
}

