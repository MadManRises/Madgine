#include "../resourceslib.h"

#include "resourcebase.h"

#include "Interfaces/filesystem/api.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Resources::ResourceBase)
READONLY_PROPERTY(Name, name)
PROPERTY(Persistent, isPersistent, setPersistent)
METATABLE_END(Engine::Resources::ResourceBase)

namespace Engine {
namespace Resources {
    ResourceBase::ResourceBase(const std::string &name, Filesystem::Path path)
        : mIsPersistent(false)
        , mName(name)
        , mPath(std::move(path))
    {
    }

    void ResourceBase::setPersistent(bool b)
    {
        mIsPersistent = b;
    }

    bool ResourceBase::isPersistent() const
    {
        return mIsPersistent;
    }

    void ResourceBase::setPath(const Filesystem::Path &path)
    {
        mPath = path;
    }

    const Filesystem::Path &ResourceBase::path()
    {
        return mPath;
    }

    std::string_view ResourceBase::extension()
    {
        return mPath.extension();
    }

    std::string_view ResourceBase::name()
    {
        return mName;
    }

    InStream ResourceBase::readAsStream(bool isBinary)
    {
        return Filesystem::openFileRead(mPath, isBinary);
    }

    std::string ResourceBase::readAsText()
    {
        InStream buffer = readAsStream();
        return std::string { buffer.iterator(), buffer.end() };
    }

    std::vector<unsigned char> Engine::Resources::ResourceBase::readAsBlob()
    {
        InStream buffer = readAsStream(true);
        return std::vector<unsigned char> { buffer.iterator(), buffer.end() };
    }

}
}
