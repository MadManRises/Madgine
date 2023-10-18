#pragma once

#include "Madgine/resources/resourceloader.h"

namespace Engine {
namespace Window {

    struct LayoutDummy {
    };

    struct MADGINE_CLIENT_EXPORT LayoutLoader : Resources::ResourceLoader<LayoutLoader, LayoutDummy> {

        LayoutLoader();

        Threading::Task<bool> loadImpl(LayoutDummy &data, ResourceDataInfo &info);
        Threading::Task<void> unloadImpl(LayoutDummy &data);
    };

}
}

REGISTER_TYPE(Engine::Window::LayoutLoader)