#include "../directx12lib.h"

#include "directx12pixelshader.h"
#include "directx12program.h"
#include "directx12vertexshader.h"

#include "Meta/math/matrix4.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

#include "directx12vertexarray.h"

namespace Engine {
namespace Render {

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

    DirectX12Program::~DirectX12Program()
    {
        reset();
    }

    DirectX12Program::DirectX12Program(DirectX12Program &&other)
        : mVertexShader(std::move(other.mVertexShader))
        , mPixelShader(std::move(other.mPixelShader))
    {
    }

    DirectX12Program &DirectX12Program::operator=(DirectX12Program &&other)
    {
        std::swap(mVertexShader, other.mVertexShader);
        std::swap(mPixelShader, other.mPixelShader);
        return *this;
    }

    bool DirectX12Program::link(typename DirectX12VertexShaderLoader::HandleType vertexShader, typename DirectX12PixelShaderLoader::HandleType pixelShader)
    {
        reset();

        /*mHandle = glCreateProgram();
        glAttachShader(mHandle, vertexShader->mHandle);
        glAttachShader(mHandle, pixelShader->mHandle);

        for (size_t i = 0; i < attributeNames.size(); ++i) {
            glBindAttribLocation(mHandle, i, attributeNames[i]);
            GL_CHECK();
        }

        glLinkProgram(mHandle);
        // check for linking errors
        GLint success;
        char infoLog[512];
        glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mHandle, 512, NULL, infoLog);
            LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED");
            LOG_ERROR(infoLog);
        }*/

        mVertexShader = std::move(vertexShader);
        mPixelShader = std::move(pixelShader);

        return true;
    }

    void DirectX12Program::reset()
    {
        mVertexShader.reset();
        mPixelShader.reset();
    }

    void DirectX12Program::bind(DirectX12VertexArray *format)
    {
        auto pib = mPipelineStates.try_emplace(format);
        if (pib.second) {

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc;
            ZeroMemory(&pipelineDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

            pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(mVertexShader->getInstance(format));
            pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(mPixelShader->mShader);

            std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayoutDesc;
            for (const AttributeDescriptor &att : format->mAttributes) {
                vertexLayoutDesc.push_back({ att.mSemantic,
                    static_cast<UINT>(att.mSemanticIndex), dxFormat(att), 0, static_cast<UINT>(att.mOffset), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            }
            pipelineDesc.InputLayout.pInputElementDescs = vertexLayoutDesc.data();
            pipelineDesc.InputLayout.NumElements = vertexLayoutDesc.size();

            pipelineDesc.pRootSignature = DirectX12RenderContext::getSingleton().mRootSignature;

            pipelineDesc.RasterizerState.AntialiasedLineEnable = FALSE;
            pipelineDesc.RasterizerState.CullMode = /*D3D12_CULL_BACK*/ D3D12_CULL_MODE_NONE;
            pipelineDesc.RasterizerState.DepthBias = 0;
            pipelineDesc.RasterizerState.DepthBiasClamp = 0.0f;
            pipelineDesc.RasterizerState.DepthClipEnable = FALSE;
            pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
            pipelineDesc.RasterizerState.FrontCounterClockwise = FALSE;
            pipelineDesc.RasterizerState.MultisampleEnable = FALSE;
            pipelineDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;

            pipelineDesc.BlendState.RenderTarget[0].BlendEnable = true;
            pipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            pipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            pipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            pipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
            pipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            pipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

            pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

            pipelineDesc.NumRenderTargets = 1;
            pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

            pipelineDesc.SampleDesc.Count = 1;
            pipelineDesc.SampleDesc.Quality = 0;

            HRESULT hr = sDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pib.first->second));
            DX12_CHECK(hr);
        }

        DirectX12RenderContext::getSingleton().mCommandList.mList->SetPipelineState(mPipelineStates[format]);

        for (size_t i = 0; i < std::min(size_t { 3 }, mConstantBuffers.size()); ++i)
            if (mConstantBuffers[i])
                DirectX12RenderContext::getSingleton().mCommandList.mList->SetGraphicsRootConstantBufferView(i, mConstantBuffers[i].gpuAddress());
        DX12_CHECK();
    }

    void DirectX12Program::setParameters(size_t index, size_t size)
    {
        if (mConstantBuffers.size() <= index)
            mConstantBuffers.resize(index + 1);

        if (!mConstantBuffers[index]) {
            mConstantBuffers[index] = { size };
        } else {
            mConstantBuffers[index].resize(size);
        }
    }

    WritableByteBuffer DirectX12Program::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

    void DirectX12Program::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mDynamicBuffers.size() <= index)
            mDynamicBuffers.resize(index + 1);

        if (!mDynamicBuffers[index]) {
            mDynamicBuffers[index] = { data.mSize };
        } else {
            mDynamicBuffers[index].resize(data.mSize);
        }

        auto target = mDynamicBuffers[index].mapData();
        std::memcpy(target.mData, data.mData, data.mSize);
    }
}
}