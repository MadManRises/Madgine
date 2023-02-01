#include "directx11lib.h"

#include "directx11vertexshaderloader.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx11shadercodegen.h"

#include "Interfaces/filesystem/fsapi.h"

UNIQUECOMPONENT(Engine::Render::DirectX11VertexShaderLoader);

namespace Engine {
namespace Render {

    std::string GetLatestVertexProfile()
    {
        assert(sDevice);

        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = sDevice->GetFeatureLevel();
        switch (featureLevel) {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "vs_5_0";
            break;
        case D3D_FEATURE_LEVEL_10_1:
            return "vs_4_1";
            break;
        case D3D_FEATURE_LEVEL_10_0:
            return "vs_4_0";
            break;
        case D3D_FEATURE_LEVEL_9_3:
            return "vs_4_0_level_9_3";
            break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "vs_4_0_level_9_1";
            break;
        default:
            return "";
        }
    }

    DirectX11VertexShaderLoader::DirectX11VertexShaderLoader()
        : ResourceLoader({ ".vs_hlsl" })
    {
    }

    Threading::TaskFuture<bool> DirectX11VertexShaderLoader::Handle::create(std::string_view name, const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader)
    {
        return Base::Handle::create(
            name, {}, [=, &file](DirectX11VertexShaderLoader *loader, DirectX11VertexShader &shader, const DirectX11VertexShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }

    bool DirectX11VertexShaderLoader::loadImpl(DirectX11VertexShader &shader, ResourceDataInfo &info)
    {
        std::string source = info.resource()->readAsText();

        return loadFromSource(shader, info.resource()->path().stem(), source);
    }

    void DirectX11VertexShaderLoader::unloadImpl(DirectX11VertexShader &shader)
    {
        shader.mShader.reset();
        shader.mBlob.reset();
    }

    bool DirectX11VertexShaderLoader::create(DirectX11VertexShader &shader, Resource *res, const CodeGen::ShaderFile &file)
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

        std::ostringstream ss;
        DirectX11ShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str());
    }

    bool DirectX11VertexShaderLoader::loadFromSource(DirectX11VertexShader &shader, std::string_view name, std::string source)
    {
        std::string profile = "latest";
        if (profile == "latest")
            profile = GetLatestVertexProfile();

        const char *cSource = source.data();

        ReleasePtr<ID3DBlob> pShaderBlob;
        ReleasePtr<ID3DBlob> pErrorBlob;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        HRESULT hr = D3DCompile(cSource, source.size(), name.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
            flags, 0, &pShaderBlob, &pErrorBlob);

        if (FAILED(hr)) {
            LOG_ERROR("Loading of VertexShader '" << name << "' failed:");
            if (pErrorBlob) {
                LOG_ERROR((char *)pErrorBlob->GetBufferPointer());
            }

            return false;
        }        
        hr = sDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &shader.mShader);
        DX11_CHECK(hr);

        shader.mBlob = std::move(pShaderBlob);

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
