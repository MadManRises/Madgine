#include "../directx12lib.h"

#include "directx12vertexarray.h"

#include "directx12buffer.h"

#include "directx12program.h"

#include "../directx12rendercontext.h"

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

    DirectX12VertexArray::DirectX12VertexArray(size_t groupSize, const DirectX12Buffer &vertex, const DirectX12Buffer &index, const std::array<AttributeDescriptor, 7> &attributes)
        : mVBO(&vertex)
        , mEBO(&index)
        , mGroupSize(groupSize)
    {
        uint8_t acc = 1;

        for (const AttributeDescriptor &attribute : attributes) {
            if (attribute) {
                mFormat += acc;

                mAttributes.push_back(attribute);
                if (mStride == 0)
                    mStride = attribute.mStride;
                else {
                    assert(mStride == attribute.mStride);
                }
            }
            acc <<= 1;
        }
    }

    DirectX12VertexArray::DirectX12VertexArray(DirectX12VertexArray &&other)
        : mStride(std::exchange(other.mStride, 0))
        , mFormat(std::exchange(other.mFormat, 0))
        , mAttributes(std::move(other.mAttributes))
        , mVBO(std::exchange(other.mVBO, nullptr))
        , mEBO(std::exchange(other.mEBO, nullptr))
        , mGroupSize(std::exchange(other.mGroupSize, 0))
    {
    }

    DirectX12VertexArray::~DirectX12VertexArray()
    {
        reset();
    }

    DirectX12VertexArray &DirectX12VertexArray::operator=(DirectX12VertexArray &&other)
    {
        mAttributes = std::move(other.mAttributes);
        mVBO = std::exchange(other.mVBO, nullptr);
        mEBO = std::exchange(other.mEBO, nullptr);
        mStride = std::exchange(other.mStride, 0);
        mFormat = std::exchange(other.mFormat, 0);
        mGroupSize = std::exchange(other.mGroupSize, 0);
        return *this;
    }

    DirectX12VertexArray::operator bool() const
    {
        return mVBO != nullptr;
    }

    void DirectX12VertexArray::reset()
    {
        mAttributes.clear();
        mVBO = 0;
        mEBO = 0;
        mFormat = 0;
    }

    void DirectX12VertexArray::bind(DirectX12Program *program, DirectX12VertexShader *vertexShader, DirectX12PixelShader *pixelShader)
    {
        constexpr D3D12_PRIMITIVE_TOPOLOGY modes[] {
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mGroupSize > 0 && mGroupSize <= 3);
        D3D12_PRIMITIVE_TOPOLOGY mode = modes[mGroupSize - 1];
        DirectX12RenderContext::getSingleton().mCommandList.mList->IASetPrimitiveTopology(mode);


        ID3D12PipelineState *&pipeline = mInstances[program];
        if (!pipeline) {

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc;
            ZeroMemory(&pipelineDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

            pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader->getInstance(this));
            pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader->mShader);

            std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayoutDesc;
            for (const AttributeDescriptor &att : mAttributes) {
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

            constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE types[] {
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
            };

            pipelineDesc.PrimitiveTopologyType = types[mGroupSize - 1];

            pipelineDesc.NumRenderTargets = 1;
            pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

            pipelineDesc.SampleDesc.Count = 1;
            pipelineDesc.SampleDesc.Quality = 0;

            HRESULT hr = sDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipeline));
            DX12_CHECK(hr);
        }

        DirectX12RenderContext::getSingleton().mCommandList.mList->SetPipelineState(pipeline);



        DX12_LOG("Bind VAO -> " << mVBO << ", " << mEBO);

        if (mVBO)
            mVBO->bindVertex(mStride);
        if (mEBO)
            mEBO->bindIndex();
    }

}
}