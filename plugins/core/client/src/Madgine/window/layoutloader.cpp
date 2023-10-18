#include "../clientlib.h"

#include "layoutloader.h"

#include "Meta/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Window::LayoutLoader)

METATABLE_BEGIN(Engine::Window::LayoutLoader)
MEMBER(mResources)
METATABLE_END(Engine::Window::LayoutLoader)

METATABLE_BEGIN_BASE(Engine::Window::LayoutLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Window::LayoutLoader::Resource)

namespace Engine {
namespace Window {

    LayoutLoader::LayoutLoader()
        : ResourceLoader({ ".layout" })
    {
    }

    Threading::Task<bool> LayoutLoader::loadImpl(LayoutDummy &data, ResourceDataInfo &info)
    {
        throw 0;
    }

    Threading::Task<void> LayoutLoader::unloadImpl(LayoutDummy &data)
    {
        throw 0;
    }

}
}