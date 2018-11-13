#include "../../interfaceslib.h"
#include "untrackedmemoryresource.h"


#ifdef _WIN32
#include <Windows.h>
#endif

namespace Engine {
	namespace Debug {
		namespace Memory {			

			StatsMemoryResource *UntrackedMemoryResource::sInstance()
			{
				static UntrackedMemoryResource resource;
				static StatsMemoryResource stats(&resource);
				return &stats;
			}

			void * UntrackedMemoryResource::do_allocate(size_t _Bytes, size_t _Align)
			{
#ifdef _WIN32			

				static HANDLE heap = GetProcessHeap();
				return HeapAlloc(heap, HEAP_GENERATE_EXCEPTIONS, _Bytes);

#else
#error "Unsupported Platform!"
#endif
			}

			void UntrackedMemoryResource::do_deallocate(void * _Ptr, size_t _Bytes, size_t _Align)
			{

#ifdef _WIN32

				static HANDLE heap = GetProcessHeap();
				auto result = HeapFree(heap, 0, _Ptr);
				assert(result);

#else
#error "Unsupported Platform!"
#endif
			}

			bool UntrackedMemoryResource::do_is_equal(const std::pmr::memory_resource & _That) const noexcept
			{
				return true;
			}

		}
	}
}


