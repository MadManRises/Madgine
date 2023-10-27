#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "../directx11pipelineloader.h"
#include "../directx11pixelshaderloader.h"
#include "../directx11vertexshaderloader.h"
#include "directx11buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11PipelineInstance : PipelineInstance {

        DirectX11PipelineInstance(const PipelineConfiguration &config, ID3D11VertexShader *vertexShader, ID3D10Blob *vertexBlob, ID3D11PixelShader *pixelShader);

        bool bind(VertexFormat format, size_t groupSize) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void renderMesh(RenderTarget *target, const GPUMeshData *mesh) const override;
        virtual void renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;

        virtual void bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;
        static void bindTexturesImpl(const std::vector<TextureDescriptor> &tex, size_t offset = 0);

    private:
        ID3D11VertexShader *mVertexShader;
        ID3D10Blob *mVertexShaderBlob;
        ID3D11PixelShader *mPixelShader;

        std::vector<DirectX11Buffer> mConstantBuffers;

        DirectX11VertexShaderLoader::Handle mVertexShaderHandle;
        DirectX11PixelShaderLoader::Handle mPixelShaderHandle;
    };

    struct MADGINE_DIRECTX11_EXPORT DirectX11PipelineInstanceHandle : DirectX11PipelineInstance {

        DirectX11PipelineInstanceHandle(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::Handle vertexShader, typename DirectX11PixelShaderLoader::Handle pixelShader);

    private:

        DirectX11VertexShaderLoader::Handle mVertexShaderHandle;
        DirectX11PixelShaderLoader::Handle mPixelShaderHandle;
    };

    struct MADGINE_DIRECTX11_EXPORT DirectX11PipelineInstancePtr : DirectX11PipelineInstance {

        DirectX11PipelineInstancePtr(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::Ptr vertexShader, typename DirectX11PixelShaderLoader::Ptr pixelShader);

    private:
        DirectX11VertexShaderLoader::Ptr mVertexShaderHandle;
        DirectX11PixelShaderLoader::Ptr mPixelShaderHandle;
    };

}
}