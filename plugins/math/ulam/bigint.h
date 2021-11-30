#pragma once

namespace Engine {

struct BigInt {

    BigInt(uint32_t v)
    {
        if (v > 0)
            mBits.push_back(v);
    }

    bool operator>(const BigInt &other) const
    {
        if (mBits.size() > other.mBits.size())
            return true;
        if (mBits.size() < other.mBits.size())
            return false;
        for (auto it1 = mBits.rbegin(), it2 = other.mBits.rbegin(); it1 != mBits.rend() && it2 != other.mBits.rend(); ++it1, ++it2) {
            if (*it1 > *it2)
                return true;
            if (*it1 < *it2)
                return false;
        }
        return false;
    }

    uint32_t operator%(uint32_t mod) const
    {
        uint64_t buffer = 0;
        for (auto it = mBits.rbegin(); it != mBits.rend(); ++it) {
            buffer <<= 32;
            buffer = (buffer + *it) % mod;
        }
        return buffer;
    }

    BigInt &operator+=(uint32_t v)
    {
        uint64_t buffer = v;
        for (uint32_t &bits : mBits) {
            buffer += bits;
            bits = buffer;
            buffer >>= 32;
            if (buffer == 0)
                return *this;
        }
        mBits.push_back(buffer);
        return *this;
    }

    BigInt &operator-=(const BigInt &v) {
        if (v.mBits.size() == 0)
            return *this;
        assert(mBits.size() >= v.mBits.size());
        uint64_t buffer = 0;
        for (size_t i = 0; i < mBits.size(); ++i) {
            if (i < v.mBits.size())
                buffer += v.mBits[i];
            bool overflow = (mBits[i] < buffer);
            mBits[i] -= buffer;
            if (!overflow && i >= v.mBits.size() - 1) {
                while (!mBits.empty() && mBits.back() == 0)
                    mBits.pop_back();
                return *this;
            }
            buffer = 1;
        }
        throw 0;
    }

    BigInt operator-(const BigInt &v) const
    {
        BigInt i = *this;
        i -= v;
        return i;
    }

    BigInt &operator/=(uint32_t v)
    {
        uint64_t buffer = 0;
        for (auto it = mBits.rbegin(); it != mBits.rend(); ++it) {
            buffer += *it;
            uint64_t rem = buffer % v;
            *it = buffer / v;
            buffer = rem;
            buffer <<= 32;
        }
        while (!mBits.empty() && mBits.back() == 0)
            mBits.pop_back();
        return *this;
    }

    BigInt& operator<<=(uint32_t v) {
        uint64_t buffer = 0;
        for (uint32_t& bits : mBits) {
            buffer += bits;
            buffer <<= v;
            bits = buffer;
            buffer >>= 32;
        }
        if (buffer > 0)
            mBits.push_back(buffer);
        return *this;
    }

    BigInt &operator*=(uint32_t v)
    {
        uint64_t buffer = 0;
        for (uint32_t &bits : mBits) {
            buffer += static_cast<uint64_t>(bits) * v;
            bits = buffer;
            buffer >>= 32;
        }
        if (buffer > 0)
            mBits.push_back(buffer);
        return *this;
    }

    BigInt &operator>>=(uint32_t v)
    {
        uint32_t buffer = 0;
        for (auto it = mBits.rbegin(); it != mBits.rend(); ++it) {
            uint32_t rem = *it & ((1 << v) - 1);
            *it >>= v;
            *it |= (buffer << (32 - v));
            buffer = rem;            
        }
        while (!mBits.empty() && mBits.back() == 0)
            mBits.pop_back();
        return *this;
    }

    explicit operator uint32_t() const {
        assert(mBits.size() <= 1);
        return mBits.empty() ? 0 : mBits[0];
    }

    std::vector<uint32_t> mBits;
};

}