#include "../directx12lib.h"

#include "directx12vertexshader.h"

#include "../directx12rendercontext.h"

#include "directx12vertexarray.h"

#include "Madgine/resources/resourcebase.h"

namespace Engine {
namespace Render {

    std::string GetLatestVertexProfile()
    {
        return "vs_5_1";
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

    DirectX12VertexShader::DirectX12VertexShader(Resources::ResourceBase *resource)
        : mResource(resource)
    {
    }

    DirectX12VertexShader::DirectX12VertexShader(DirectX12VertexShader &&other)
        : mResource(std::exchange(other.mResource, nullptr))
        , mInstances(std::move(other.mInstances))
    {
    }

    DirectX12VertexShader::~DirectX12VertexShader()
    {
        reset();
    }

    DirectX12VertexShader &DirectX12VertexShader::operator=(DirectX12VertexShader &&other)
    {
        std::swap(mResource, other.mResource);
        std::swap(mInstances, other.mInstances);
        return *this;
    }

    void DirectX12VertexShader::reset()
    {
        for (std::pair<ID3DBlob *, std::vector<D3D12_INPUT_ELEMENT_DESC>> &p : mInstances) {
            if (p.first) {
                assert(!p.second.empty());
                p.first->Release();
                p.first = nullptr;
                p.second.clear();
            }
        }
    }

    /*void DirectX12VertexShader::bind(DirectX12VertexArray *format)
    {
        uint8_t index = format->mFormat;
        if (mInstances.size() <= index)
            mInstances.resize(index + 1);

        if (!mInstances[index].first) {

            std::string_view filename = mResource->path().stem();

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

            std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayoutDesc;

            std::list<std::string> shaderMacroBuffer;
            std::vector<D3D_SHADER_MACRO> shaderMacros;

            std::stringstream ss;

            for (const AttributeDescriptor &att : format->mAttributes) {
                vertexLayoutDesc.push_back({ att.mSemantic,
                    static_cast<UINT>(att.mSemanticIndex), dxFormat(att), 0, static_cast<UINT>(att.mOffset), D3D12_INPUT_PER_VERTEX_DATA, 0 });

                ss.str("");
                ss << "HAS_" << att.mSemantic << att.mSemanticIndex;
                shaderMacroBuffer.push_back(ss.str());
                shaderMacros.push_back({ shaderMacroBuffer.back().c_str(), nullptr });
            }
            shaderMacros.push_back({ nullptr, nullptr });

            HRESULT hr = D3DCompile(cSource, source.size(), mResource->path().c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
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

                std::terminate();
            }

            hr = sDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &mInstances[index].first);
            DX12_CHECK(hr);
            
            hr = sDevice->CreateInputLayout(vertexLayoutDesc.data(), vertexLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &mInstances[index].second);
            DX12_CHECK(hr);

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
    }*/

}
}