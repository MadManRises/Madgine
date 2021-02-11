#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx11pixelshader.h"

namespace Engine {
namespace Render {

    struct DirectX11PixelShaderLoader : Resources::ResourceLoader<DirectX11PixelShaderLoader, DirectX11PixelShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11PixelShaderLoader();

        bool loadImpl(DirectX11PixelShader &shader, ResourceType *res);
        void unloadImpl(DirectX11PixelShader &shader, ResourceType *res);
    };

}
}

RegisterType(Engine::Render::DirectX11PixelShaderLoader);