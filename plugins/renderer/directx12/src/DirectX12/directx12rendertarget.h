#pragma once

#include "Madgine/render/rendertarget.h"


namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderTarget : RenderTarget {

        DirectX12RenderTarget(DirectX12RenderContext *context, bool global, std::string name);
        ~DirectX12RenderTarget();

        void setup(OffsetPtr targetView, const Vector2i &size);
        void shutdown();

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(const Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<uint32_t> indices = {}) override;
        virtual void renderVertices(const Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<uint32_t> indices = {}, const Material *material = nullptr) override;
        virtual void renderMesh(const GPUMeshData *mesh, const Program *program, const Material *material = nullptr) override;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const Program *program, const Material *material = nullptr) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;        

        DirectX12RenderContext *context() const;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        mutable OffsetPtr mTargetView;
        ID3D12Resource *mDepthStencilBuffer = nullptr;  
        OffsetPtr mDepthStencilView;
        D3D12_DEPTH_STENCIL_DESC mDepthStencilStateDesc;
        D3D12_RASTERIZER_DESC mRasterizerDesc;
        D3D12_BLEND_DESC mBlendState;

        //ID3D12SamplerState *mSamplers[2] = { nullptr, nullptr };
        
    };

}
}