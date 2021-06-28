#pragma once

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Resources {

    struct MADGINE_RESOURCES_EXPORT ResourceBase {
        ResourceBase(const std::string &name, Filesystem::Path path = {});

        ~ResourceBase() noexcept = default;

        void setPersistent(bool b);

        bool isPersistent() const;

        void setPath(const Filesystem::Path &path);
        const Filesystem::Path &path();
        std::string_view extension();
        std::string_view name();

        InStream readAsStream(bool isBinary = false);
        std::string readAsText();
        std::vector<unsigned char> readAsBlob();

        static constexpr const char sUnnamed[] = "__unnamed__";

    private:
        bool mIsPersistent;

        std::string mName;
        Filesystem::Path mPath;
    };

}
}