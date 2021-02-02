#pragma once

namespace Engine {

template <size_t bits>
using MinimalHoldingUIntType = std::enable_if<bits <= 64, std::conditional_t<(bits > 32), uint64_t, std::conditional_t<(bits > 16), uint32_t, std::conditional_t<(bits > 8), uint16_t, std::conditional_t<(bits > 1), uint8_t, bool>>>>>;

template <size_t bits>
using MinimalHoldingUIntType_t = typename MinimalHoldingUIntType<bits>::type;

constexpr size_t bitSize(size_t v)
{
    size_t result = 0;
    while (v) {
        ++result;
        v >>= 1;
    }
    return result;
}

template <size_t Count, size_t MemberSize = 1, typename type = MinimalHoldingUIntType_t<MemberSize>>
struct BitArray {

    static constexpr uint64_t Mask = (1 << MemberSize) - 1;

    constexpr BitArray() = default;

    static constexpr uint64_t readPtrAsUInt64(const uint8_t *ptr, size_t offset)
    {
        uint64_t result = 0;
        size_t dist = (offset + MemberSize - (offset / 8) * 8 - 1) / 8 + 1;
        for (int i = dist - 1; i >= 0; --i) {
            result <<= 8;
            result += ptr[offset / 8 + i];
        }
        return result;
    }

    static constexpr void writePtrAsUInt64(uint8_t *ptr, size_t offset, uint64_t value)
    {
        size_t dist = (offset + MemberSize - (offset / 8) * 8 - 1) / 8 + 1;
        for (int i = 0; i < dist; ++i) {
            ptr[offset / 8 + i] = value;
            value >>= 8;
        }
    }

    struct const_reference {

        operator type() const
        {

            uint64_t value = readPtrAsUInt64(mPointer, mOffset) >> (mOffset % 8);
            value &= Mask;
            return static_cast<type>(value);
        }

        const uint8_t *mPointer;
        size_t mOffset;
    };

    struct reference {
        constexpr reference operator=(type v)
        {
            uint64_t newMask = Mask << (mOffset % 8);
            uint64_t newValue = static_cast<uint64_t>(v);
            newValue <<= (mOffset % 8);
            writePtrAsUInt64(mPointer, mOffset, (readPtrAsUInt64(mPointer, mOffset) & ~newMask) | newValue);
            return *this;
        }

        operator type() const
        {
            uint64_t value = readPtrAsUInt64(mPointer, mOffset) >> (mOffset % 8);
            value &= Mask;
            return static_cast<type>(value);
        }

        uint8_t *mPointer;
        size_t mOffset;
    };

    struct const_pointer {

        const_reference operator*() const
        {
            return { mPointer, mOffset };
        }

        void operator++()
        {
            mOffset += MemberSize;
        }

        bool operator!=(const const_pointer &other) const
        {
            assert(mPointer == other.mPointer);
            return mOffset != other.mOffset;
        }

        const uint8_t *mPointer;
        size_t mOffset;
    };

    struct pointer {

        reference operator*() const
        {
            return { mPointer, mOffset };
        }

        void operator++()
        {
            mOffset += MemberSize;
        }

        bool operator!=(const pointer &other) const
        {
            assert(mPointer == other.mPointer);
            return mOffset != other.mOffset;
        }

        uint8_t *mPointer;
        size_t mOffset;
    };

    constexpr reference operator[](size_t i)
    {
        assert(i < Count);
        return { mData, i * MemberSize };
    }

    constexpr const_reference operator[](size_t i) const
    {
        assert(i < Count);
        return { mData, i * MemberSize };
    }

    const_pointer begin() const
    {
        return { mData, 0 };
    }

    const_pointer end() const
    {
        return { mData, Count * MemberSize };
    }

    pointer begin()
    {
        return { mData, 0 };
    }

    pointer end()
    {
        return { mData, Count * MemberSize };
    }

    bool operator==(const BitArray &other) const
    {
        return mData == other.mData;
    }

    bool operator!=(const BitArray &other) const
    {
        return !(*this == other);
    }

private:
    uint8_t mData[(Count * MemberSize - 1) / 8 + 1] = {};
};

}