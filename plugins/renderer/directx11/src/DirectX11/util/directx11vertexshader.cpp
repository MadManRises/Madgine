#include "../directx11lib.h"

#include "directx11vertexshader.h"

#include "../directx11rendercontext.h"

#include "directx11vertexarray.h"

#include "Modules/resources/resourcebase.h"

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

    DXGI_FORMAT dxFormat(const AttributeDescriptor &desc)
    {
        switch (desc.mType) {
        case ATTRIBUTE_FLOAT: {
            constexpr DXGI_FORMAT formats[] = {
                DXGI_FORMAT_R32_FLOAT,
                DXGI_FORMAT_R32G32_FLOAT,
                DXGI_FORMAT_R32G32B32_FLOAT,
                DXGI_FORMAT_R32G32B32A32_FLOAT
            };
            return formats[desc.mArraySize - 1];
        }
        case ATTRIBUTE_INT: {
            constexpr DXGI_FORMAT formats[] = {
                DXGI_FORMAT_R32_SINT,
                DXGI_FORMAT_R32G32_SINT,
                DXGI_FORMAT_R32G32B32_SINT,
                DXGI_FORMAT_R32G32B32A32_SINT
            };
            return formats[desc.mArraySize - 1];
        }
        }
        std::terminate();
    }

    DirectX11VertexShader::DirectX11VertexShader(Resources::ResourceBase *resource)
        : mResource(resource)
    {
    }

    DirectX11VertexShader::DirectX11VertexShader(DirectX11VertexShader &&other)
        : mResource(std::exchange(other.mResource, nullptr))
        , mInstances(std::move(other.mInstances))
    {
    }

    DirectX11VertexShader::~DirectX11VertexShader()
    {
        reset();
    }

    DirectX11VertexShader &DirectX11VertexShader::operator=(DirectX11VertexShader &&other)
    {
        std::swap(mResource, other.mResource);
        std::swap(mInstances, other.mInstances);
        return *this;
    }

    void DirectX11VertexShader::reset()
    {
        for (std::pair<ID3D11VertexShader *, ID3D11InputLayout *> &p : mInstances) {
            if (p.first) {
                assert(p.second);
                p.first->Release();
                p.second->Release();
                p = { nullptr,
                    nullptr };
            }
        }
    }

    void DirectX11VertexShader::bind(DirectX11VertexArray *format)
    {
        uint8_t index = format->mFormat;
        if (mInstances.size() <= index)
            mInstances.resize(index + 1);

        if (!mInstances[index].first) {

            std::string filename = mResource->path().stem();

            std::string profile = "latest";
            if (profile == "latest")
                profile = GetLatestVertexProfile();

            std::string source = mResource->readAsText();

            const char *cSource = source.c_str();

            ID3DBlob *pShaderBlob = nullptr;
            ID3DBlob *pErrorBlob = nullptr;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
            flags |= D3DCOMPILE_DEBUG;
#endif

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexLayoutDesc;

            std::list<std::string> shaderMacroBuffer;
            std::vector<D3D_SHADER_MACRO> shaderMacros;

            std::stringstream ss;

            for (const AttributeDescriptor &att : format->mAttributes) {
                vertexLayoutDesc.push_back({ att.mSemantic,
                    static_cast<UINT>(att.mSemanticIndex), dxFormat(att), 0, static_cast<UINT>(att.mOffset), D3D11_INPUT_PER_VERTEX_DATA, 0 });

                ss.str("");
                ss << "HAS_" << att.mSemantic << att.mSemanticIndex;
                shaderMacroBuffer.push_back(ss.str());
                shaderMacros.push_back({ shaderMacroBuffer.back().c_str(), nullptr });
            }
            shaderMacros.push_back({ nullptr, nullptr });

            HRESULT hr = D3DCompile(cSource, source.size(), mResource->path().c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
                flags, 0, &pShaderBlob, &pErrorBlob);

            if (FAILED(hr)) {
                LOG_ERROR("Loading of Shader '"s + filename + "' failed:");
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

                std::terminate();
            }

            hr = sDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &mInstances[index].first);
            DX11_CHECK(hr);

            hr = sDevice->CreateInputLayout(vertexLayoutDesc.data(), vertexLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &mInstances[index].second);
            DX11_CHECK(hr);

            if (pShaderBlob) {
                pShaderBlob->Release();
                pShaderBlob = nullptr;
            }
            if (pErrorBlob) {
                pErrorBlob->Release();
                pErrorBlob = nullptr;
            }
        }

        sDeviceContext->VSSetShader(mInstances[index].first, nullptr, 0);
        sDeviceContext->IASetInputLayout(mInstances[index].second);
    }

}
}