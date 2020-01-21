#include "directx11lib.h"

#include "directx11vertexshaderloader.h"

#include "Interfaces/stringutil.h"

#include "util/directx11vertexshader.h"

#include "directx11rendercontext.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

UNIQUECOMPONENT(Engine::Render::DirectX11VertexShaderLoader);

namespace Engine {
namespace Render {

    DirectX11VertexShaderLoader::DirectX11VertexShaderLoader()
        : ResourceLoader({ ".VS_hlsl" })
    {
    }

    bool DirectX11VertexShaderLoader::loadImpl(DirectX11VertexShader &shader, ResourceType *res)
    {
        shader = { res };

        return true;
    }

    void DirectX11VertexShaderLoader::unloadImpl(DirectX11VertexShader &shader, ResourceType *res)
    {
        shader.reset();
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11VertexShaderLoader)
METATABLE_END(Engine::Render::DirectX11VertexShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11VertexShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11VertexShaderLoader::ResourceType)

RegisterType(Engine::Render::DirectX11VertexShaderLoader);
