#include "../directx11lib.h"

#include "directx11vertexshaderlist.h"

#include "../directx11rendercontext.h"

#include "Madgine/resources/resourcebase.h"

#include "Madgine/render/shadinglanguage/slloader.h"

#include "Madgine/codegen/resolveincludes.h"

namespace Engine {
namespace Render {

    std::string GetLatestVertexProfile()
    {
        assert(sDevice);

        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = sDevice->GetFeatureLevel();

        switch (featureLevel) {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0: {
            return "vs_5_0";
        } break;
        case D3D_FEATURE_LEVEL_10_1: {
            return "vs_4_1";
        } break;
        case D3D_FEATURE_LEVEL_10_0: {
            return "vs_4_0";
        } break;
        case D3D_FEATURE_LEVEL_9_3: {
            return "vs_4_0_level_9_3";
        } break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1: {
            return "vs_4_0_level_9_1";
        } break;
        } // switch( featureLevel )

        return "";
    }

    static constexpr const char *vMacros[] = {
        "HAS_POS_3D",
        "HAS_POS_4D",
        "HAS_POS2",
        "HAS_NORMAL",
        "HAS_COLOR",
        "HAS_UV",
        "HAS_BONE_INDICES",
        "HAS_BONE_WEIGHTS"
    };

    ReleasePtr<ID3D11VertexShader> &DirectX11VertexShaderList::get(VertexFormat format, size_t instanceDataSize)
    {
        ReleasePtr<ID3D11VertexShader> &shader = mShaders[format];
        if (!shader) {

            std::string_view filename = mResource->path().stem();

            std::string profile = "latest";
            if (profile == "latest")
                profile = GetLatestVertexProfile();

            std::string source = mResource->readAsText();

            std::map<std::string, size_t> files;

            CodeGen::resolveIncludes(
                source, [](const Filesystem::Path &path, size_t line, std::string_view filename) {
                    Resources::ResourceBase *res = SlLoader::get(path.stem());
                    return "#line 1 \"" + path.filename().str() + "\"\n" + res->readAsText() + "\n#line " + std::to_string(line + 1) + " \"" + std::string { filename } + "\"";
                },
                filename, files);

            const char *cSource = source.c_str();

            ReleasePtr<ID3DBlob> pShaderBlob;
            ReleasePtr<ID3DBlob> pErrorBlob;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
            flags |= D3DCOMPILE_DEBUG;
#endif

            std::vector<D3D_SHADER_MACRO> shaderMacros;

            for (size_t i = 0; i < VertexElements::size; ++i) {
                if (format.has(i))
                    shaderMacros.push_back({ vMacros[i], nullptr });
            }
            shaderMacros.push_back({ nullptr, nullptr });

            HRESULT hr = D3DCompile(cSource, source.size(), mResource->path().c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
                flags, 0, &pShaderBlob, &pErrorBlob);

            if (FAILED(hr)) {
                LOG_ERROR("Loading of VertexShader '" << filename << "' failed:");
                if (pErrorBlob) {
                    LOG_ERROR((char *)pErrorBlob->GetBufferPointer());
                }

                std::terminate();
            }

            static_cast<DirectX11RenderContext &>(RenderContext::getSingleton()).ensureFormat(format, instanceDataSize, pShaderBlob);

            hr = sDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &shader);
            DX11_CHECK(hr);
        }

        return shader;
    }

    void DirectX11VertexShaderList::reset()
    {
        for (ReleasePtr<ID3D11VertexShader> &shader : mShaders) {
            shader.reset();
        }
    }

    const ReleasePtr<ID3D11VertexShader> *DirectX11VertexShaderList::ptr() const
    {
        return mShaders.data();
    }

}
}