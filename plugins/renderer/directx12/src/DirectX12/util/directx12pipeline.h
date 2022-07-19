#pragma once

#include "pipeline.h"

#include "../directx12geometryshaderloader.h"
#include "../directx12pixelshaderloader.h"
#include "../directx12vertexshaderloader.h"

#include "render/vertexformat.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12Pipeline : Pipeline {

        bool link(std::string_view vertexShader, typename DirectX12GeometryShaderLoader::Handle geometryShader, typename DirectX12PixelShaderLoader::Handle pixelShader);

        ID3D12PipelineState *get(VertexFormat format, size_t groupSize, size_t instanceDataSize);

        std::array<ReleasePtr<ID3D12PipelineState>, 3> *ptr();

        void reset();

    private:
        std::array<std::array<ReleasePtr<ID3D12PipelineState>, 3>, 256> mPipelines;
        std::array<DirectX12VertexShaderLoader::Handle, 256> mVertexShaders;

        std::string mVertexShader;
        typename DirectX12GeometryShaderLoader::Handle mGeometryShader;
        typename DirectX12PixelShaderLoader::Handle mPixelShader;
    };

}
}