#pragma once

#include "Madgine/render/rendertarget.h"

#include "util/directx12commandlist.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderTarget : RenderTarget {

        DirectX12RenderTarget(DirectX12RenderContext *context, bool global, std::string name, RenderTarget *blitSource = nullptr);
        ~DirectX12RenderTarget();

        void setup(std::vector<OffsetPtr> targetViews, const Vector2i &size, size_t samples = 1, bool createDepthBufferView = false);
        void shutdown();

        void beginFrame() override;
        void endFrame() override;

        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const override;
        virtual void endIteration() const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;

        virtual void clearDepthBuffer() override;

        DirectX12RenderContext *context() const;

        size_t samples() const;


        mutable DirectX12CommandList mCommandList;
        mutable std::vector<OffsetPtr> mTargetViews;
        ReleasePtr<ID3D12Resource> mDepthStencilBuffer;  
        OffsetPtr mDepthStencilView;
        D3D12_DEPTH_STENCIL_DESC mDepthStencilStateDesc;
        D3D12_RASTERIZER_DESC mRasterizerDesc;
        D3D12_BLEND_DESC mBlendState;
        D3D12_RESOURCE_STATES mResourceState;

        size_t mSamples;
    };

}
}