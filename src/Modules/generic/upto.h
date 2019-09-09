#pragma once

namespace Engine {

template <typename T, size_t Count>
struct UpTo {

    UpTo() = default;

    UpTo(std::initializer_list<T> elements)
    {
        for (const T &t : elements)
            push_back(t);
    }

    ~UpTo()
    {
        while (mCount > 0) {
            --mCount;
            mData[mCount].~T();
        }
    }

    void push_back(const T &t)
    {
        assert(mCount < Count);
        new (mData + mCount) T(t);
        ++mCount;
    }

    T &operator[](size_t i)
    {
        assert(i < Count);
        assert(i < mCount);
        return mData[i];
    }

    const T &operator[](size_t i) const
    {
        assert(i < Count);
        assert(i < mCount);
        return mData[i];
    }

    operator bool() const
    {
        return mCount > 0;
    }

    size_t size() const
    {
		return mCount;
	}

private:
    union {
        char _dummy;
        T mData[Count];
    };

    size_t mCount = 0;
};

}