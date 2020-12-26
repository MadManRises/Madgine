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

    std::string_view ResourceBase::extension()
    {
        return mPath.extension();
    }

    std::string_view ResourceBase::name()
    {
        return mPath.stem();
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
