#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "../directx11geometryshaderloader.h"
#include "../directx11pipelineloader.h"
#include "../directx11pixelshaderloader.h"
#include "../directx11vertexshaderloader.h"
#include "directx11buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11PipelineInstance : PipelineInstance {

        DirectX11PipelineInstance(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::Handle vertexShader, typename DirectX11PixelShaderLoader::Handle pixelShader, typename DirectX11GeometryShaderLoader::Handle geometryShader, bool dynamic);

        bool bind(VertexFormat format, size_t groupSize) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) override;

        virtual void renderMesh(const GPUMeshData *mesh) const override;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;
        static void bindTexturesImpl(const std::vector<TextureDescriptor> &tex, size_t offset = 0);

    private:
        union {
            ID3D11VertexShader *mVertexShader;
            const ReleasePtr<ID3D11VertexShader> *mVertexShaders;
        };
        ID3D11PixelShader *mPixelShader;
        ID3D11GeometryShader *mGeometryShader;

        std::vector<DirectX11Buffer> mConstantBuffers;
        std::vector<DirectX11Buffer> mDynamicBuffers;

        DirectX11VertexShaderLoader::Handle mVertexShaderHandle;
        DirectX11PixelShaderLoader::Handle mPixelShaderHandle;
        DirectX11GeometryShaderLoader::Handle mGeometryShaderHandle;
        
        bool mIsDynamic;
    };

}
}