#pragma once

namespace Engine {

struct OffsetPtr {

    constexpr OffsetPtr() = default;

    explicit constexpr OffsetPtr(uintptr_t offset)
        : mOffset(offset)
    {
    }

    constexpr uintptr_t offset() const {
        return mOffset;
    }

    constexpr void reset() {
        mOffset = std::numeric_limits<uintptr_t>::max();
    }

    constexpr std::strong_ordering operator<=>(const OffsetPtr &other) const = default;
    constexpr std::strong_ordering operator<=>(uintptr_t offset) const {
        return mOffset <=> offset;
    }

    constexpr explicit operator bool() const {
        return mOffset != std::numeric_limits<uintptr_t>::max();
    }

    constexpr OffsetPtr& operator+=(uintptr_t offset) {
        mOffset += offset;
        return *this;
    }

    friend constexpr const std::byte *operator+(const std::byte *p, OffsetPtr offset)
    {
        return p + offset.mOffset;
    }

    friend constexpr const std::byte *operator-(const std::byte *p, OffsetPtr offset)
    {
        return p - offset.mOffset;
    }

    friend constexpr std::byte *operator+(std::byte *p, OffsetPtr offset)
    {
        return p + offset.mOffset;
    }

    friend constexpr std::byte *operator-(std::byte *p, OffsetPtr offset)
    {
        return p - offset.mOffset;
    }

    constexpr OffsetPtr operator+(uintptr_t offset) const {
        return OffsetPtr{ mOffset + offset };
    }

    friend constexpr OffsetPtr operator+(uintptr_t offset, OffsetPtr ptr) {
        return ptr + offset;
    }

private:
    uintptr_t mOffset = std::numeric_limits<uintptr_t>::max();
};


}