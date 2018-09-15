#pragma once

namespace Engine {
	namespace Core {

		struct AllocationData {
			AllocationData(size_t s) : mSize(s) {}
			size_t mSize;
		};

		struct INTERFACES_EXPORT MemoryTracker {		
			static MemoryTracker &sInstance();		

			void *allocate(size_t s);
			void deallocate(void *p);

			size_t overhead();
			size_t rawMemory();
			size_t registeredMemory();

		private:
			MemoryTracker();
			~MemoryTracker();

			//SAFE ALLOC
			char* mPageBase;
			size_t mVirtualMemUsage;

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
			void onMalloc(size_t s);
			void onFree(void *p, size_t s);

			std::atomic<size_t> mRegisteredMemory;
			std::atomic<size_t> mRawMemory;

			std::mutex mMutex;
			std::unordered_map<void *, AllocationData, std::hash<void*>, std::equal_to<void*>, UntrackedAllocator<std::pair<void *const, AllocationData>>> mAllocations;
			struct BackTrace {
				BackTrace(std::vector<void*, UntrackedAllocator<void*>> &&trace) : mTrace(std::move(trace)) {}
				std::vector<void*, UntrackedAllocator<void*>> mTrace;
			};
			std::unordered_map<unsigned long, BackTrace, std::hash<unsigned long>, std::equal_to<unsigned long>, UntrackedAllocator<std::pair<const unsigned long, BackTrace>>> mBacktraces;

		public:
			const std::unordered_map<unsigned long, BackTrace, std::hash<unsigned long>, std::equal_to<unsigned long>, UntrackedAllocator<std::pair<const unsigned long, BackTrace>>> &backtraces();
		private:

			//STATS IMPL
#ifdef _WIN32
			int(*mOldHook)(int, void*, size_t, int, long, const unsigned char *, int);

			static int win32Hook(int allocType, void *userData, size_t size, int
				blockType, long requestNumber, const unsigned char *filename, int
				lineNumber);

			struct ThreadLocalStorage {
				static thread_local bool sRequesting;
				static thread_local bool sTryLock;
		};
#else
#error "Unsupported Platform!"
#endif

			//ORIGINAL
			static void *original_new(size_t s);
			static void original_delete(void *p);


			static MemoryTracker &sInitializer;




		};



	}
}

