#pragma once

#include "Interfaces/filesystem/path.h"

#include "../keyvalue/scopebase.h"

namespace Engine {
namespace Resources {

    struct MODULES_EXPORT ResourceBase : ScopeBase {
        ResourceBase(Filesystem::Path path = {});

        ~ResourceBase() = default;

        void setPersistent(bool b);

        bool isPersistent() const;

        const Filesystem::Path &path();
        std::string extension();
        std::string name();

        void updatePath(const Filesystem::Path &path) { LOG_WARNING("Implement updatePath!"); } //TODO

        std::string readAsText();
        std::vector<unsigned char> readAsBlob();

    private:
        bool mIsPersistent;

        Filesystem::Path mPath;
    };

}
}