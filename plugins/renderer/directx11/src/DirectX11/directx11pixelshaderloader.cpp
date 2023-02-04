#include "directx11lib.h"

#include "directx11pixelshaderloader.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx11shadercodegen.h"

#include "Interfaces/filesystem/fsapi.h"

UNIQUECOMPONENT(Engine::Render::DirectX11PixelShaderLoader);

namespace Engine {
namespace Render {

    std::string GetLatestPixelProfile()
    {
        assert(sDevice);

        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = sDevice->GetFeatureLevel();
        switch (featureLevel) {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "ps_5_0";
            break;
        case D3D_FEATURE_LEVEL_10_1:
            return "ps_4_1";
            break;
        case D3D_FEATURE_LEVEL_10_0:
            return "ps_4_0";
            break;
        case D3D_FEATURE_LEVEL_9_3:
            return "ps_4_0_level_9_3";
            break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "ps_4_0_level_9_1";
            break;
        default:
            return "";
        }
    }

    DirectX11PixelShaderLoader::DirectX11PixelShaderLoader()
        : ResourceLoader({ ".ps_hlsl" })
    {
    }

    Threading::TaskFuture<bool> DirectX11PixelShaderLoader::Handle::create(std::string_view name, const CodeGen::ShaderFile &file, DirectX11PixelShaderLoader *loader)
    {
        return Base::Handle::create(
            name, {}, [=, &file](DirectX11PixelShaderLoader *loader, ReleasePtr<ID3D11PixelShader> &shader, const DirectX11PixelShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }

    bool DirectX11PixelShaderLoader::loadImpl(ReleasePtr<ID3D11PixelShader> &shader, ResourceDataInfo &info)
    {
        std::string source = info.resource()->readAsText();

        return loadFromSource(shader, info.resource()->path().stem(), source);
    }

    void DirectX11PixelShaderLoader::unloadImpl(ReleasePtr<ID3D11PixelShader> &shader)
    {
        shader.reset();
    }

    bool DirectX11PixelShaderLoader::create(ReleasePtr<ID3D11PixelShader> &shader, Resource *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx11";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".PS_hlsl"));
        }

        std::ostringstream ss;
        DirectX11ShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str());
    }

    bool DirectX11PixelShaderLoader::loadFromSource(ReleasePtr<ID3D11PixelShader> &shader, std::string_view name, std::string source)
    {
        std::string profile = "latest";
        if (profile == "latest")
            profile = GetLatestPixelProfile();

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
            LOG_ERROR("Loading of PixelShader '" << name << "' failed:");
            if (pErrorBlob) {
                LOG_ERROR((char *)pErrorBlob->GetBufferPointer());
            }

            return false;
        }

        hr = sDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &shader);
        DX11_CHECK(hr);

        return true;
    }

    Threading::TaskQueue *DirectX11PixelShaderLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11PixelShaderLoader)
METATABLE_END(Engine::Render::DirectX11PixelShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PixelShaderLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11PixelShaderLoader::Resource)
