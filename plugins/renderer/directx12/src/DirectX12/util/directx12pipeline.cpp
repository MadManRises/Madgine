#include "../directx12lib.h"

#include "directx12pipeline.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    bool DirectX12Pipeline::link(typename DirectX12VertexShaderLoader::Handle vertexShader, typename DirectX12PixelShaderLoader::Handle pixelShader)
    {
        reset();

        mVertexShader = std::move(vertexShader);
        mPixelShader = std::move(pixelShader);

        return true;
    }

    bool DirectX12Pipeline::link(typename DirectX12VertexShaderLoader::Ptr vertexShader, typename DirectX12PixelShaderLoader::Ptr pixelShader)
    {
        reset();

        mVertexShader = std::move(vertexShader);
        mPixelShader = std::move(pixelShader);

        return true;
    }

    ID3D12PipelineState *DirectX12Pipeline::get(VertexFormat format, size_t groupSize, size_t samples, size_t instanceDataSize) const
    {
        size_t samplesBits = sqrt(samples);
        assert(samplesBits * samplesBits == samples);

        ReleasePtr<ID3D12PipelineState> &pipeline = mPipelines[format][groupSize - 1][samplesBits - 1];

        if (!pipeline) {

            auto checkVertex = [](auto &variant) {
                return std::visit(overloaded {
                                      [](const typename DirectX12VertexShaderLoader::Handle &handle) {
                                          return !handle || handle.available();
                                      },
                                      [](const typename DirectX12VertexShaderLoader::Ptr &ptr) {
                                          return true;
                                      } },
                    variant);
            };
            auto checkPixel = [](auto &variant) {
                return std::visit(overloaded {
                                      [](const typename DirectX12PixelShaderLoader::Handle &handle) {
                                          return !handle || handle.available();
                                      },
                                      [](const typename DirectX12PixelShaderLoader::Ptr &ptr) {
                                          return true;
                                      } },
                    variant);
            };

            auto resolve = [](auto &variant) -> IDxcBlob * {
                return std::visit(
                    [](const auto &handle) -> IDxcBlob * {
                        return handle ? *handle : nullptr;
                    },
                    variant);
            };

            if (!checkVertex(mVertexShader))
                return nullptr;
            if (!checkPixel(mPixelShader))
                return nullptr;

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc;
            ZeroMemory(&pipelineDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

            pipelineDesc.VS = { resolve(mVertexShader)->GetBufferPointer(), resolve(mVertexShader)->GetBufferSize() };
            if (resolve(mPixelShader))
                pipelineDesc.PS = { resolve(mPixelShader)->GetBufferPointer(), resolve(mPixelShader)->GetBufferSize() };

            std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayoutDesc = DirectX12RenderContext::createVertexLayout(format, instanceDataSize);

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
            pipelineDesc.RasterizerState.ForcedSampleCount = 0;
            pipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

            pipelineDesc.BlendState.AlphaToCoverageEnable = TRUE;
            pipelineDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
            pipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            pipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            pipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            pipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
            pipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            pipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

            pipelineDesc.DepthStencilState.DepthEnable = TRUE;
            pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            pipelineDesc.DepthStencilState.StencilEnable = FALSE;
            pipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

            constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE types[] {
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
            };

            pipelineDesc.PrimitiveTopologyType = types[groupSize - 1];

            pipelineDesc.NodeMask = 1;

            pipelineDesc.SampleMask = UINT_MAX;

            pipelineDesc.NumRenderTargets = 1;
            pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

            pipelineDesc.SampleDesc.Count = samples;
            pipelineDesc.SampleDesc.Quality = 0;

            HRESULT hr = GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipeline));
            DX12_CHECK(hr);
        }

        return pipeline;
    }

    const std::array<std::array<ReleasePtr<ID3D12PipelineState>, 3>, 3> *DirectX12Pipeline::ptr() const
    {
        return mPipelines.data();
    }

    void DirectX12Pipeline::reset()
    {
        for (std::array<std::array<ReleasePtr<ID3D12PipelineState>, 3>, 3> &groupPipelines : mPipelines)
            for (std::array<ReleasePtr<ID3D12PipelineState>, 3> &samplePipelines : groupPipelines)
                for (ReleasePtr<ID3D12PipelineState> &pipeline : samplePipelines)
                    pipeline.reset();
        mVertexShader = {};
        mPixelShader = {};
    }

}
}