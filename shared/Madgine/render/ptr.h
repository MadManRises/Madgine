#pragma once

namespace Engine {
namespace Render {

    struct UniqueOpaquePtr {

        UniqueOpaquePtr() = default;
        UniqueOpaquePtr(const UniqueOpaquePtr &) = delete;
        UniqueOpaquePtr(UniqueOpaquePtr &&other)
            : mPtr(std::exchange(other.mPtr, 0))
        {
        }
        ~UniqueOpaquePtr()
        {
            if (mPtr)
                std::terminate();
        }

        UniqueOpaquePtr &operator=(const UniqueOpaquePtr &) = delete;
        UniqueOpaquePtr &operator=(UniqueOpaquePtr &&other)
        {
            std::swap(mPtr, other.mPtr);
            return *this;
        }

        constexpr explicit operator bool() const
        {
            return mPtr;
        }

        uintptr_t get() const
        {
            return mPtr;
        }

        template <typename T>
        T &setupAs()
        {
            assert(!mPtr);
            return reinterpret_cast<T&>(mPtr);
        }

        template <typename T>
        operator T() const
        {
            assert(mPtr);
            return (T)(mPtr);
        }

        template <typename T>
        T release() {
            assert(mPtr);
            return (T)(std::exchange(mPtr, 0));
        }

    private:
        uintptr_t mPtr = 0;
    };

    template <typename T>
    struct GPUPtr {

        GPUPtr() = default;
        template <typename U>
        requires std::convertible_to<U *, T *>
        GPUPtr(GPUPtr<U> other)
            : mOffset(other.mOffset)
            , mBuffer(other.mBuffer)
        {
        }
        template <typename U>
        explicit GPUPtr(GPUPtr<U> other)
            : mOffset(other.mOffset)
            , mBuffer(other.mBuffer)
        {
        }

        uint32_t mOffset = 0;
        IndexType<uint32_t, 0> mBuffer;
    };

    template <typename T>
    using GPUArrayPtr = GPUPtr<T[]>;

}
}