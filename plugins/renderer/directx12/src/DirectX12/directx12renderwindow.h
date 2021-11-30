#pragma once

#include "directx12rendertarget.h"

#include <dxgi.h>

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderWindow : DirectX12RenderTarget {
        DirectX12RenderWindow(DirectX12RenderContext *context, Window::OSWindow *w);
        DirectX12RenderWindow(const DirectX12RenderWindow &) = delete;
        ~DirectX12RenderWindow();

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;

        virtual bool resizeImpl(const Vector2i &size) override;
        virtual Vector2i size() const override;

    protected:
        void createRenderTargetViews();

    private:
        Window::OSWindow *mWindow;

        IDXGISwapChain3 *mSwapChain;

        OffsetPtr mTargetViews[2];
        ID3D12Resource *mBackBuffers[2];
    };

}
}

RegisterType(Engine::Render::DirectX12RenderWindow);