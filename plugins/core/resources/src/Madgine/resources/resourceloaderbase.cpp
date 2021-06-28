#include "../resourceslib.h"

#include "resourceloaderbase.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Resources::ResourceLoaderBase)
METATABLE_END(Engine::Resources::ResourceLoaderBase)

namespace Engine {
namespace Resources {

    ResourceLoaderBase::ResourceLoaderBase(std::vector<std::string> &&extensions, const ResourceLoaderSettings &settings)
        : mExtensions(std::forward<std::vector<std::string>>(extensions))
        , mSettings(settings)
    {
    }

    const std::vector<std::string> &ResourceLoaderBase::fileExtensions() const
    {
        return mExtensions;
    }

    size_t ResourceLoaderBase::extensionIndex(std::string_view ext) const
    {
        return std::find(mExtensions.begin(), mExtensions.end(), ext) - mExtensions.begin();
    }

    std::vector<const MetaTable *> ResourceLoaderBase::resourceTypes() const
    {
        return {};
    }
}
}

