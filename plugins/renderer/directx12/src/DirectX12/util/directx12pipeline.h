#pragma once

#include "Madgine/pipelineloader/pipeline.h"

#include "../directx12geometryshaderloader.h"
#include "../directx12pixelshaderloader.h"
#include "../directx12vertexshaderloader.h"

#include "Madgine/render/vertexformat.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12Pipeline : Pipeline {

        bool link(typename DirectX12VertexShaderLoader::Handle vertexShader, typename DirectX12GeometryShaderLoader::Handle geometryShader, typename DirectX12PixelShaderLoader::Handle pixelShader);

        ID3D12PipelineState *get(VertexFormat format, size_t groupSize, size_t instanceDataSize);

        std::array<ReleasePtr<ID3D12PipelineState>, 3> *ptr();

        void reset();

    private:
        std::array<std::array<ReleasePtr<ID3D12PipelineState>, 3>, 256> mPipelines;

        typename DirectX12VertexShaderLoader::Handle mVertexShader;
        typename DirectX12GeometryShaderLoader::Handle mGeometryShader;
        typename DirectX12PixelShaderLoader::Handle mPixelShader;
    };

}
}