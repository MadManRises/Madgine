#include "directx12lib.h"

#include "directx12vertexshadersourceloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx12shadercodegen.h"

#include "Interfaces/filesystem/api.h"

#include "directx12rendercontext.h"

UNIQUECOMPONENT(Engine::Render::DirectX12VertexShaderSourceLoader);

METATABLE_BEGIN(Engine::Render::DirectX12VertexShaderSourceLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::DirectX12VertexShaderSourceLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12VertexShaderSourceLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX12VertexShaderSourceLoader::Resource)

namespace Engine {
namespace Render {

    DirectX12VertexShaderSourceLoader::DirectX12VertexShaderSourceLoader()
        : ResourceLoader({ ".VS_hlsl" })
    {
    }

    /* void DirectX12VertexShaderLoader::Handle::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12VertexShaderLoader *loader)
    {
        *this = DirectX12VertexShaderLoader::loadManual(
            name, {}, [=, &file](DirectX12VertexShaderLoader *loader, DirectX12VertexShader &shader, const DirectX12VertexShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }*/

    bool DirectX12VertexShaderSourceLoader::loadImpl(std::string &shader, ResourceDataInfo &info)
    {
        shader = info.resource()->readAsText();

        return true;
    }

    void DirectX12VertexShaderSourceLoader::unloadImpl(std::string &shader)
    {
        shader.clear();
    }

    /* bool DirectX12VertexShaderLoader::create(DirectX12VertexShader &shader, Resource *res, const CodeGen::ShaderFile &file)
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
    }*/

    Threading::TaskQueue *DirectX12VertexShaderSourceLoader::loadingTaskQueue() const
    {
        return DirectX12RenderContext::renderQueue();
    }
}
}
