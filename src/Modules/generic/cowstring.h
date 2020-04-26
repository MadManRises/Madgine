#pragma once

namespace Engine {

struct CoWString {
    CoWString() = default;

    CoWString(const std::string &s)
        : mString(s.c_str())
        , mSize(s.size())
    {
    }

    CoWString(const std::string &&s)
        : mSize(s.size())
    {
        char *temp = new char[mSize];
        strncpy(temp, s.c_str(), mSize);
        mString = temp;
        mOwning = true;
    }

    CoWString(std::string &&s)
        : CoWString(static_cast<const std::string &&>(s))
    {
    }

    CoWString(const std::string_view &s)
        : mString(s.data())
        , mSize(s.size())
    {
    }

    CoWString(const CoWString &other)
        : mString(other.mString)
        , mSize(other.mSize)
    {
    }

    CoWString(CoWString &&other)
        : mString(std::exchange(other.mString, nullptr))
        , mSize(std::exchange(other.mSize, 0))
        , mOwning(std::exchange(other.mOwning, false))
    {
    }

    ~CoWString()
    {
        reset();
    }

    CoWString &operator=(const CoWString &other)
    {
        reset();
        mString = other.mString;
        mSize = other.mSize;
        return *this;
    }

    CoWString &operator=(CoWString &&other)
    {
        std::swap(mString, other.mString);
        std::swap(mSize, other.mSize);
        std::swap(mOwning, other.mOwning);
        return *this;
    }

    CoWString &operator=(const std::string &s)
    {
        reset();
        mString = s.c_str();
        mSize = s.size();
        return *this;
    }

    CoWString &operator=(const std::string &&s)
    {
        reset();
        char *temp = new char[mSize];
        mString = temp;
        mOwning = true;
        mSize = s.size();
        strncpy(temp, s.c_str(), mSize);
        return *this;
    }

    CoWString &operator=(std::string &&s)
    {
        (*this) = static_cast<const std::string &&>(s);
        return *this;
    }

    CoWString &operator=(const std::string_view &s)
    {
        reset();
        mString = s.data();
        mSize = s.size();
        return *this;
    }

    bool operator==(const CoWString &other) const
    {
        if (mSize != other.mSize)
            return false;
        if (mString == other.mString)
            return true;
        return strncmp(mString, other.mString, mSize) == 0;
    }

    bool operator<(const CoWString &other) const
    {
        if (!other.mString)
            return false;
        if (!mString)
            return true;
        if (mSize < other.mSize)
            return strncmp(mString, other.mString, mSize) <= 0;
        else
            return strncmp(mString, other.mString, other.mSize) < 0;
    }

    operator std::string_view() const &
    {
        return { mString, mSize };
    }

    operator std::string() const
    {
        return { mString, mSize };
    }

    void reset()
    {
        if (mOwning) {
            delete[] mString;
            mString = nullptr;
            mSize = 0;
            mOwning = false;
        }
    }

    constexpr const char *data() const
    {
        return mString;
    }

    constexpr size_t size() const
    {
        return mSize;
    }

private:
    const char *mString = nullptr;
    unsigned int mSize = 0;
    bool mOwning = false;
};

}