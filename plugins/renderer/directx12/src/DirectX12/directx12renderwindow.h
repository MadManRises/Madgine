#pragma once

#include "directx12rendertarget.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderWindow : DirectX12RenderTarget {
        DirectX12RenderWindow(DirectX12RenderContext *context, Window::OSWindow *w, size_t samples);
        DirectX12RenderWindow(const DirectX12RenderWindow &) = delete;
        ~DirectX12RenderWindow();

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual bool resizeImpl(const Vector2i &size) override;
        virtual Vector2i size() const override;

    protected:
        void createRenderTargetViews();

    private:
        Window::OSWindow *mWindow;

        ReleasePtr<IDXGISwapChain3> mSwapChain;

        OffsetPtr mTargetViews[2];
        ReleasePtr<ID3D12Resource> mBackBuffers[2];
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX12RenderWindow)