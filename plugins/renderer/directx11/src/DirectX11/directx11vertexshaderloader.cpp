#include "directx11lib.h"

#include "directx11vertexshaderloader.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx11shadercodegen.h"

#include "Interfaces/filesystem/api.h"

UNIQUECOMPONENT(Engine::Render::DirectX11VertexShaderLoader);

namespace Engine {
namespace Render {

    DirectX11VertexShaderLoader::DirectX11VertexShaderLoader()
        : ResourceLoader({ ".vs_hlsl" })
    {
    }

    Threading::TaskFuture<bool> DirectX11VertexShaderLoader::Handle::create(std::string_view name, const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader)
    {
        return Base::Handle::create(
            name, {}, [=, &file](DirectX11VertexShaderLoader *loader, DirectX11VertexShaderList &shader, const DirectX11VertexShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }

    bool DirectX11VertexShaderLoader::loadImpl(DirectX11VertexShaderList &list, ResourceDataInfo &info)
    {
        list.mResource = info.resource();

        return true;
    }

    void DirectX11VertexShaderLoader::unloadImpl(DirectX11VertexShaderList &list)
    {
        list.reset();
    }

    bool DirectX11VertexShaderLoader::create(DirectX11VertexShaderList &shader, Resource *res, const CodeGen::ShaderFile &file)
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

    Threading::TaskQueue *DirectX11VertexShaderLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11VertexShaderLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::DirectX11VertexShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11VertexShaderLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11VertexShaderLoader::Resource)
