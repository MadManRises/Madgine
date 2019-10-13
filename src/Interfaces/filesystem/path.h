#pragma once

namespace Engine {
namespace Filesystem {

    struct INTERFACES_EXPORT Path {

        Path(const std::string &s);
        Path(const char *s = "");

		Path &operator=(const std::string &s);
        Path &operator=(const char *s);

        Path &operator/=(const Path &p);

        Path operator/(const Path &p) const
        {
            Path p2 = *this;
            p2 /= p;
            return p2;
        }

        bool operator==(const Path &other) const;
        bool operator!=(const Path &other) const;

        Path parentPath() const;
        Path relative() const;
        Path relative(const Path &base) const;
        Path absolute() const;
        Path absolute(const Path &base) const;

		void normalize();

        Path filename() const;
        std::string stem() const;
        std::string extension() const;

        bool isAbsolute() const;
        bool isRelative() const;

        bool empty() const;
        const std::string &str() const;
        const char *c_str() const;

    private:
        std::string mPath;
    };

    inline std::ostream &operator<<(std::ostream &out, const Path &p)
    {
        return out << p.str();
    }

}
}