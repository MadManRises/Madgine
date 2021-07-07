#include "directx11lib.h"

#include "directx11pixelshaderloader.h"

#include "Generic/stringutil.h"

#include "util/directx11pixelshader.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx11shadercodegen.h"

#include "Interfaces/filesystem/api.h"

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
        : ResourceLoader({ ".PS_hlsl" })
    {
    }

    void DirectX11PixelShaderLoader::HandleType::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11PixelShaderLoader *loader)
    {
        *this = DirectX11PixelShaderLoader::loadManual(
            name, {}, [=, &file](DirectX11PixelShaderLoader *loader, DirectX11PixelShader &shader, const DirectX11PixelShaderLoader::ResourceDataInfo &info) mutable { return loader->create(shader, info.resource(), file); }, {}, loader);
    }

    bool DirectX11PixelShaderLoader::loadImpl(DirectX11PixelShader &shader, ResourceDataInfo &info)
    {
        std::string source = info.resource()->readAsText();

        return loadFromSource(shader, info.resource()->path().stem(), source);
    }

    void DirectX11PixelShaderLoader::unloadImpl(DirectX11PixelShader &shader, ResourceDataInfo &info)
    {
        shader.reset();
    }

    bool DirectX11PixelShaderLoader::create(DirectX11PixelShader &shader, ResourceType *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx11";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".PS_hlsl"));
        }

        std::stringstream ss;
        DirectX11ShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str());
    }

    bool DirectX11PixelShaderLoader::loadFromSource(DirectX11PixelShader &shader, std::string_view name, std::string_view source)
    {
        std::string profile = "latest";
        if (profile == "latest")
            profile = GetLatestPixelProfile();

        const char *cSource = source.data();

        ID3DBlob *pShaderBlob = nullptr;
        ID3DBlob *pErrorBlob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        HRESULT hr = D3DCompile(cSource, source.size(), name.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
            flags, 0, &pShaderBlob, &pErrorBlob);

        if (FAILED(hr)) {
            LOG_ERROR("Loading of Shader '" << name << "' failed:");
            if (pErrorBlob) {
                LOG_ERROR((char *)pErrorBlob->GetBufferPointer());

                if (pShaderBlob) {
                    pShaderBlob->Release();
                    pShaderBlob = nullptr;
                }
                if (pErrorBlob) {
                    pErrorBlob->Release();
                    pErrorBlob = nullptr;
                }
            }

            return false;
        }

        shader = { pShaderBlob };

        if (pShaderBlob) {
            pShaderBlob->Release();
            pShaderBlob = nullptr;
        }
        if (pErrorBlob) {
            pErrorBlob->Release();
            pErrorBlob = nullptr;
        }

        return true;
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11PixelShaderLoader)
METATABLE_END(Engine::Render::DirectX11PixelShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PixelShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11PixelShaderLoader::ResourceType)
