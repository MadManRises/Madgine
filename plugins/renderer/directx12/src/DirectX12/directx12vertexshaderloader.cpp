#include "directx12lib.h"

#include "directx12vertexshaderloader.h"

#include "util/directx12vertexshader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx12shadercodegen.h"

#include "Interfaces/filesystem/api.h"

UNIQUECOMPONENT(Engine::Render::DirectX12VertexShaderLoader);

namespace Engine {
namespace Render {

    DirectX12VertexShaderLoader::DirectX12VertexShaderLoader()
        : ResourceLoader({ ".VS_hlsl" })
    {
    }

    void DirectX12VertexShaderLoader::HandleType::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12VertexShaderLoader *loader)
    {
        *this = DirectX12VertexShaderLoader::loadManual(
            name, {}, [=, &file](DirectX12VertexShaderLoader *loader, DirectX12VertexShader &shader, const DirectX12VertexShaderLoader::ResourceDataInfo &info) mutable { return loader->create(shader, info.resource(), file); }, {}, loader);
    }

    bool DirectX12VertexShaderLoader::loadImpl(DirectX12VertexShader &shader, ResourceDataInfo &info)
    {
        shader = { info.resource() };

        return true;
    }

    void DirectX12VertexShaderLoader::unloadImpl(DirectX12VertexShader &shader, ResourceDataInfo &info)
    {
        shader.reset();
    }

    bool DirectX12VertexShaderLoader::create(DirectX12VertexShader &shader, ResourceType *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx12";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".VS_hlsl"));
        }

        {
            std::ofstream f { res->path() };
            DirectX12ShaderCodeGen::generate(f, file, 0);
        }
        
        shader = { res };

        return true;
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX12VertexShaderLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::DirectX12VertexShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12VertexShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX12VertexShaderLoader::ResourceType)
