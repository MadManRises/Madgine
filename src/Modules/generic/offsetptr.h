#pragma once

namespace Engine {

	
template <typename T, typename M>
struct OffsetPtr {
    template <typename Target>
    OffsetPtr(Target(T::*P))
    {
        mOffset = reinterpret_cast<size_t>(static_cast<M *>(&(static_cast<T *>(nullptr)->*P)));
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
        offset += reinterpret_cast<size_t>(static_cast<T *>(static_cast<_T *>(nullptr)));
        offset += reinterpret_cast<size_t>(static_cast<_M *>(static_cast<M *>(nullptr)));
        return offset;
	}

    size_t mOffset;
};

}