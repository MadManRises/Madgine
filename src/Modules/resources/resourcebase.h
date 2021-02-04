#pragma once

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Resources {

    struct MODULES_EXPORT ResourceBase {
        ResourceBase(const std::string &name, Filesystem::Path path = {});

        ~ResourceBase() = default;

        void setPersistent(bool b);

        bool isPersistent() const;

        const Filesystem::Path &path();
        std::string_view extension();
        std::string_view name();

        void updatePath(const Filesystem::Path &path) { LOG_WARNING("Implement updatePath!"); } //TODO

        InStream readAsStream(bool isBinary = false);
        std::string readAsText();
        std::vector<unsigned char> readAsBlob();

    private:
        bool mIsPersistent;

        std::string mName;
        Filesystem::Path mPath;
    };

}
}