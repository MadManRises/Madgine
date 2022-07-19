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
        virtual void renderMesh(const GPUMeshData *mesh, const PipelineInstance *pipeline, const Material *material = nullptr) override;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const PipelineInstance *pipeline, const Material *material = nullptr) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;        

        DirectX12RenderContext *context() const;

        static void TransitionBarrier(ID3D12GraphicsCommandList *commandList, ID3D12Resource *res, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to);
        void Transition(ID3D12Resource *res, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) const;


        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        mutable OffsetPtr mTargetView;
        ReleasePtr<ID3D12Resource> mDepthStencilBuffer;  
        OffsetPtr mDepthStencilView;
        D3D12_DEPTH_STENCIL_DESC mDepthStencilStateDesc;
        D3D12_RASTERIZER_DESC mRasterizerDesc;
        D3D12_BLEND_DESC mBlendState;
        D3D12_RESOURCE_STATES mResourceState;

        //ID3D12SamplerState *mSamplers[2] = { nullptr, nullptr };
        
    };

}
}