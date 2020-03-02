#include "../moduleslib.h"

#include "resourcebase.h"

#include "Interfaces/filesystem/api.h"

#include "../keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Resources::ResourceBase)
READONLY_PROPERTY(Name, name)
PROPERTY(Persistent, isPersistent, setPersistent)
METATABLE_END(Engine::Resources::ResourceBase)

namespace Engine {
namespace Resources {
    ResourceBase::ResourceBase(Filesystem::Path path)
        : mIsPersistent(false)
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

    const Filesystem::Path &ResourceBase::path()
    {
        return mPath;
    }

    std::string ResourceBase::extension()
    {
        return mPath.extension();
    }

    std::string ResourceBase::name()
    {
        return mPath.stem();
    }

    std::string ResourceBase::readAsText()
    {
        InStream buffer = Filesystem::openFileRead(mPath);
        return std::string { buffer.iterator(), buffer.end() };
    }

    std::vector<unsigned char> Engine::Resources::ResourceBase::readAsBlob()
    {
        InStream buffer = Filesystem::openFileRead(mPath, true);
        return std::vector<unsigned char> { buffer.iterator(), buffer.end() };
    }

}
}
