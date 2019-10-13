#include "../interfaceslib.h"
#include "path.h"
#include "api.h"

namespace Engine {
namespace Filesystem {

    Path::Path(const std::string &s)
        : mPath(s)
    {
        normalize();
    }

    Path::Path(const char *s)
        : mPath(s)
    {
        normalize();
    }

    Path &Path::operator=(const std::string &s)
    {
        mPath = s;
        normalize();
        return *this;
    }

    Path &Path::operator=(const char *s)
    {
        mPath = s;
        normalize();
        return *this;
    }

    Path &Path::operator/=(const Path &other)
    {
        assert(other.isRelative());
        if (!mPath.empty()) {
            mPath.reserve(mPath.size() + other.mPath.size() + 1);
            mPath.push_back('/');
        }
        mPath.append(other.mPath);
        normalize();
        return *this;
    }

    bool Path::operator==(const Path &other) const
    {
        return mPath == other.mPath;
    }

    bool Path::operator!=(const Path &other) const
    {
        return mPath != other.mPath;
    }

    Path Path::parentPath() const
    {
        auto sep = mPath.rfind('/');
        if (sep == std::string::npos)
            return {};
        std::string result = mPath.substr(0, sep);
        if (result.empty())
            result = "/";
        return result;
    }

    Path Path::relative() const
    {
        return relative(getCwd());
    }

    Path Path::relative(const Path &base) const
    {
        size_t baseCount = base.mPath.size();
        size_t count = mPath.size();

        if (baseCount >= count)
            return {};

        if (!isEqual(mPath.substr(0, baseCount), base.mPath))
            return {};

        if (baseCount == count)
            return ".";

        if (mPath[baseCount] != '/')
            return {};

        return mPath.substr(baseCount + 1);
    }

    Path Path::absolute() const
    {
        return absolute(getCwd());
    }

    Path Path::absolute(const Path &base) const
    {
        if (isAbsolute())
            return *this;
        else
            return base / *this;
    }

    void Path::normalize()
    {
        std::string lastElement;

        std::string s = std::move(mPath);

        size_t size = s.size();
        mPath.reserve(size);
        bool hadSeparator = false;
        for (size_t i = 0; i < size; ++i) {
            if (isSeparator(s[i])) {
                if (!hadSeparator) {
                    hadSeparator = true;
                    if (lastElement == ".") {
                        if (!mPath.empty())
							mPath.resize(mPath.size() - 1);
                    } else if (lastElement == "..") {
                        if (!mPath.empty()) {
                            mPath.resize(mPath.size() - 1);
                            mPath.resize(mPath.rfind('/') + 1);
                        }
                    } else {
                        mPath += lastElement + "/";
                    }
                    lastElement.clear();
                }
            } else {
                hadSeparator = false;
                lastElement.push_back(s[i]);
            }
        }

        if (!lastElement.empty()) {
            if (lastElement == "." && !mPath.empty()) {
                mPath.resize(mPath.size() - 1);
            } else if (lastElement == ".." && !mPath.empty()) {
                mPath.resize(mPath.size() - 1);
                size_t pos = mPath.rfind('/');
                if (pos != std::string::npos) {
                    mPath.resize(pos);
                } else {
                    mPath.clear();
				}
            } else {
                mPath += lastElement;
            }
        }

        if (hadSeparator && mPath.size() > 1) {
            mPath.pop_back();
        }
    }

    Path Path::filename() const
    {
        auto sep = mPath.rfind('/');
        return sep == std::string::npos ? mPath : mPath.substr(sep + 1);
    }

    std::string Path::stem() const
    {
        std::string name = filename().mPath;
        auto pos = name.rfind('.');
        return pos == std::string::npos ? name : name.substr(0, pos);
    }

    std::string Path::extension() const
    {
        std::string name = filename().mPath;
        auto pos = name.rfind('.');
        return pos == std::string::npos ? std::string {} : name.substr(pos);
    }

    bool Path::isAbsolute() const
    {
        return Filesystem::isAbsolute(*this);
    }

    bool Path::isRelative() const
    {
        return !isAbsolute();
    }

    bool Path::empty() const
    {
        return mPath.empty();
    }

    const std::string &Path::str() const
    {
        return mPath;
    }

    const char *Path::c_str() const
    {
        return mPath.c_str();
    }

}
}