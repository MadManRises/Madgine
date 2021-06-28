#include "directx11lib.h"

#include "directx11vertexshaderloader.h"

#include "Generic/stringutil.h"

#include "util/directx11vertexshader.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx11shadercodegen.h"

#include "Interfaces/filesystem/api.h"

UNIQUECOMPONENT(Engine::Render::DirectX11VertexShaderLoader);

namespace Engine {
namespace Render {

    DirectX11VertexShaderLoader::DirectX11VertexShaderLoader()
        : ResourceLoader({ ".VS_hlsl" })
    {
    }

    void DirectX11VertexShaderLoader::HandleType::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader)
    {
        *this = DirectX11VertexShaderLoader::loadManual(
            name, {}, [=, &file](DirectX11VertexShaderLoader *loader, DirectX11VertexShader &shader, const DirectX11VertexShaderLoader::ResourceDataInfo &info) mutable { return loader->create(shader, info.resource(), file); }, {}, loader);
    }

    bool DirectX11VertexShaderLoader::loadImpl(DirectX11VertexShader &shader, ResourceDataInfo &info)
    {
        shader = { info.resource() };

        return true;
    }

    void DirectX11VertexShaderLoader::unloadImpl(DirectX11VertexShader &shader, ResourceDataInfo &info)
    {
        shader.reset();
    }

    bool DirectX11VertexShaderLoader::create(DirectX11VertexShader &shader, ResourceType *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx11";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".VS_hlsl"));
        }

        {
            std::ofstream f { res->path() };
            DirectX11ShaderCodeGen::generate(f, file, 0);
        }
        
        shader = { res };

        return true;
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11VertexShaderLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::DirectX11VertexShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11VertexShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11VertexShaderLoader::ResourceType)
