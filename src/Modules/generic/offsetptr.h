#pragma once

namespace Engine {

	
template <typename T, typename M>
struct OffsetPtr {
    OffsetPtr() = default;

    template <typename Target>
    OffsetPtr(Target(T::*P))
    {
        mOffset = reinterpret_cast<size_t>(&static_cast<M &>(static_cast<T *>(reinterpret_cast<void *>(0x1))->*P)) - 1;
    }

    T *parent(M *m)
    {
        return reinterpret_cast<T *>(reinterpret_cast<char *>(m) - mOffset);
    }

	const T* parent(const M* m) 
	{
        return reinterpret_cast<const T *>(reinterpret_cast<const char *>(m) - mOffset);
	}

	template <typename _T = T, typename _M = M>
	size_t offset() {
        size_t offset = mOffset;
        offset += reinterpret_cast<size_t>(static_cast<T *>(static_cast<_T *>(reinterpret_cast<void*>(0x1)))) - 1;
        offset += reinterpret_cast<size_t>(static_cast<_M *>(static_cast<M *>(reinterpret_cast<void *>(0x1)))) - 1;
        return offset;
	}

    size_t mOffset = -1;
};

}