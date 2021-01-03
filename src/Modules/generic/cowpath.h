#pragma once

#include "cowstring.h"
#include "Interfaces/filesystem/path.h"

namespace Engine {

struct CoWPath {
    
    CoWPath() = default;
    
    CoWPath(const Filesystem::Path &p)
        : mData(p.str())
    {
    }

    CoWPath(const Filesystem::Path &&p)
        : mData(static_cast<const std::string &&>(p.str()))
    {
    }

    CoWPath(Filesystem::Path &&p)
        : CoWPath(static_cast<const Filesystem::Path &&>(p))
    {
    }

    CoWPath &operator=(const Filesystem::Path &p)
    {
        mData = p.str();
        return *this;
    }

    CoWPath &operator=(const Filesystem::Path &&p)
    {
        mData = static_cast<const std::string &&>(p.str());
        return *this;
    }

    CoWPath &operator=(Filesystem::Path &&p)
    {
        (*this) = static_cast<const Filesystem::Path &&>(p);
        return *this;
    }

    bool operator==(const CoWPath& other) const {
        return mData == other.mData;
    }

    operator Filesystem::Path () const
    {
        return { mData };
    }

    void reset()
    {
        mData.reset();
    }

    constexpr const char *data() const
    {
        return mData.data();
    }

    constexpr size_t size() const
    {
        return mData.size();
    }

    std::string str() const {
        return mData;
    }

private:
    CoWString mData;
};

}