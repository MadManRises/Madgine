#include "directx11lib.h"

#include "directx11pixelshaderloader.h"

#include "Interfaces/util/stringutil.h"

#include "util/directx11pixelshader.h"

#include "directx11rendercontext.h"

#include "Modules/keyvalue/metatable_impl.h"

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
        case D3D_FEATURE_LEVEL_11_0: {
            return "ps_5_0";
        } break;
        case D3D_FEATURE_LEVEL_10_1: {
            return "ps_4_1";
        } break;
        case D3D_FEATURE_LEVEL_10_0: {
            return "ps_4_0";
        } break;
        case D3D_FEATURE_LEVEL_9_3: {
            return "ps_4_0_level_9_3";
        } break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1: {
            return "ps_4_0_level_9_1";
        } break;
        }
        return "";
    }

    DirectX11PixelShaderLoader::DirectX11PixelShaderLoader()
        : ResourceLoader({ ".PS_hlsl" })
    {
    }

    bool DirectX11PixelShaderLoader::loadImpl(DirectX11PixelShader &shader, ResourceType *res)
    {
        std::string_view filename = res->path().stem();

        std::string profile = "latest";
        if (profile == "latest")
            profile = GetLatestPixelProfile();

        std::string source = res->readAsText();

        const char *cSource = source.c_str();

        ID3DBlob *pShaderBlob = nullptr;
        ID3DBlob *pErrorBlob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        HRESULT hr = D3DCompile(cSource, source.size(), res->path().c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
            flags, 0, &pShaderBlob, &pErrorBlob);

        if (FAILED(hr)) {
            LOG_ERROR("Loading of Shader '" << filename << "' failed:");
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

    void DirectX11PixelShaderLoader::unloadImpl(DirectX11PixelShader &shader, ResourceType *res)
    {
        shader.reset();
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11PixelShaderLoader)
METATABLE_END(Engine::Render::DirectX11PixelShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PixelShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11PixelShaderLoader::ResourceType)


