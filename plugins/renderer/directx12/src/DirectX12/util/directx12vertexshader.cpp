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
        for (ID3DBlob *&p : mInstances) {
            if (p) {
                p->Release();
                p = nullptr;
            }
        }
    }

    ID3DBlob *DirectX12VertexShader::getInstance(DirectX12VertexArray *format)
    {
        uint8_t index = format->mFormat;
        if (mInstances.size() <= index)
            mInstances.resize(index + 1);

        if (!mInstances[index]) {

            std::string_view filename = mResource->path().stem();

            std::string profile = "latest";
            if (profile == "latest")
                profile = GetLatestVertexProfile();

            std::string source = mResource->readAsText();

            const char *cSource = source.c_str();

            ID3DBlob *pErrorBlob = nullptr;

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
            flags |= D3DCOMPILE_DEBUG;
#endif

            

            std::list<std::string> shaderMacroBuffer;
            std::vector<D3D_SHADER_MACRO> shaderMacros;

            std::stringstream ss;

            for (const AttributeDescriptor &att : format->mAttributes) {
                ss.str("");
                ss << "HAS_" << att.mSemantic << att.mSemanticIndex;
                shaderMacroBuffer.push_back(ss.str());
                shaderMacros.push_back({ shaderMacroBuffer.back().c_str(), nullptr });
            }
            shaderMacros.push_back({ nullptr, nullptr });

            HRESULT hr = D3DCompile(cSource, source.size(), mResource->path().c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
                flags, 0, &mInstances[index], &pErrorBlob);

            if (FAILED(hr)) {
                LOG_ERROR("Loading of Shader '" << filename << "' failed:");
                if (pErrorBlob) {
                    LOG_ERROR((char *)pErrorBlob->GetBufferPointer());

                    if (mInstances[index]) {
                        mInstances[index]->Release();
                        mInstances[index] = nullptr;
                    }
                    if (pErrorBlob) {
                        pErrorBlob->Release();
                        pErrorBlob = nullptr;
                    }
                }

                std::terminate();
            }

            if (pErrorBlob) {
                pErrorBlob->Release();
                pErrorBlob = nullptr;
            }
        }
        return mInstances[index];
    }

}
}