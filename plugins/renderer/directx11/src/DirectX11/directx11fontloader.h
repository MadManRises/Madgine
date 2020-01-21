#pragma once

#include "fontloader.h"

#include "directx11fontdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11FontLoader : Resources::VirtualResourceLoaderImpl<DirectX11FontLoader, DirectX11FontData, FontLoader>{
        DirectX11FontLoader();

        bool loadImpl(DirectX11FontData &font, ResourceType *res);
        void unloadImpl(DirectX11FontData &font, ResourceType *res);

    };

}
}