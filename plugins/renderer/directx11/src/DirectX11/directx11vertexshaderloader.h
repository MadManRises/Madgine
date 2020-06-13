#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx11vertexshader.h"

namespace Engine {
namespace Render {

    struct DirectX11VertexShaderLoader : Resources::ResourceLoader<DirectX11VertexShaderLoader, DirectX11VertexShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11VertexShaderLoader();

        bool loadImpl(DirectX11VertexShader &shader, ResourceType *res);
        void unloadImpl(DirectX11VertexShader &shader, ResourceType *res);
    };

}
}

RegisterType(Engine::Render::DirectX11VertexShaderLoader);