#pragma once

#include "Generic/allocator/concepts.h"

namespace Engine {
namespace Render {

    struct DirectX12HeapAllocator {

        static constexpr size_t goodSize = 128 * 1024 * 1024; //128MB

        DirectX12HeapAllocator();

        Block allocate(size_t size);
        void deallocate(Block block);

        ID3D12Heap *heap(size_t index);

    private:
        std::vector<ReleasePtr<ID3D12Heap>> mHeaps;
    };

    struct DirectX12MappedHeapAllocator {

        static constexpr size_t goodSize = 512 * 1024; //128MB

        DirectX12MappedHeapAllocator(D3D12_HEAP_TYPE type);

        Block allocate(size_t size);
        void deallocate(Block block);

        std::pair<ID3D12Resource *, size_t> resolve(void *ptr);

    private:
        D3D12_HEAP_TYPE mType;
        struct Page {
            ReleasePtr<ID3D12Resource> mResource;
            void *mMappedAddress;
        };
        std::vector<Page> mPages;
    };

}
}