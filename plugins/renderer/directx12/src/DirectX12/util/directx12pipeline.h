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
        bool link(typename DirectX12VertexShaderLoader::Ptr vertexShader, typename DirectX12GeometryShaderLoader::Ptr geometryShader, typename DirectX12PixelShaderLoader::Ptr pixelShader);

        ID3D12PipelineState *get(VertexFormat format, size_t groupSize, size_t instanceDataSize) const;

        const std::array<ReleasePtr<ID3D12PipelineState>, 3> *ptr() const;

        void reset();

    private:
        mutable std::array<std::array<ReleasePtr<ID3D12PipelineState>, 3>, 256> mPipelines;

        std::variant<typename DirectX12VertexShaderLoader::Handle, typename DirectX12VertexShaderLoader::Ptr> mVertexShader;
        std::variant<typename DirectX12GeometryShaderLoader::Handle, typename DirectX12GeometryShaderLoader::Ptr> mGeometryShader;
        std::variant<typename DirectX12PixelShaderLoader::Handle, typename DirectX12PixelShaderLoader::Ptr> mPixelShader;
    };

}
}