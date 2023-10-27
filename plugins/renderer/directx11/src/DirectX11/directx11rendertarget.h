#pragma once

#include "Madgine/render/rendertarget.h"
#include "Meta/math/matrix4.h"

#include "Madgine/render/rendertextureconfig.h"

#include "util/directx11texture.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTarget : RenderTarget {

        DirectX11RenderTarget(DirectX11RenderContext *context, bool global, std::string name, bool flipFlop = false, RenderTarget *blitSource = nullptr);
        ~DirectX11RenderTarget();

        void setup(std::vector<std::array<ReleasePtr<ID3D11RenderTargetView>, 6>> targetViews, const Vector2i &size, TextureType type, size_t samples = 1);
        void shutdown();

        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const override;
        virtual void endIteration() const override;

        virtual void beginFrame() override;

        virtual void clearDepthBuffer() override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;

        DirectX11RenderContext *context() const;

        std::vector<std::array<ReleasePtr<ID3D11RenderTargetView>, 6>> mTargetViews;
        DirectX11Texture mDepthBuffer;
        ReleasePtr<ID3D11DepthStencilView> mDepthStencilViews[6];
        ReleasePtr<ID3D11DepthStencilState> mDepthStencilState;
        ReleasePtr<ID3D11RasterizerState> mRasterizerState;
        ReleasePtr<ID3D11BlendState> mBlendState;

        ReleasePtr<ID3D11SamplerState> mSamplers[2];
    };

}
}