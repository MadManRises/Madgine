#pragma once

struct ReleaseDeleter {
    template <typename T>
    void operator()(T *ptr)
    {
        ptr->Release();
    }
};

template <typename T>
struct ReleasePtr : std::unique_ptr<T, ReleaseDeleter> {
    using std::unique_ptr<T, ReleaseDeleter>::unique_ptr;

    T **operator&()
    {
        assert(!*this);
        return reinterpret_cast<T **>(this);
    }

    T *const *operator&() const
    {
        return reinterpret_cast<T *const *>(this);
    }

    operator T *() const
    {
        return this->get();
    }
};

typedef void *HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)

struct INTERFACES_EXPORT UniqueHandle {

    UniqueHandle() = default;
    UniqueHandle(HANDLE handle)
        : mHandle(handle)
    {
    }
    UniqueHandle(const UniqueHandle &) = delete;
    UniqueHandle(UniqueHandle &&other)
        : mHandle(std::exchange(other.mHandle, INVALID_HANDLE_VALUE))
    {
    }

    ~UniqueHandle();

    UniqueHandle &operator=(const UniqueHandle &) = delete;
    UniqueHandle& operator=(UniqueHandle&& other) {
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    operator HANDLE() const {
        return mHandle;
    }


private:
    HANDLE mHandle = INVALID_HANDLE_VALUE;
};