#pragma once

#include "directx12rendertarget.h"

#include "util/directx12buffer.h"
#include "util/directx12program.h"
#include "util/directx12texture.h"
#include "util/directx12vertexarray.h"

#include "Meta/math/atlas2.h"

#include <dxgi.h>

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderWindow : DirectX12RenderTarget {
        DirectX12RenderWindow(DirectX12RenderContext *context, Window::OSWindow *w);
        DirectX12RenderWindow(const DirectX12RenderWindow &) = delete;
        ~DirectX12RenderWindow();

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual Texture *texture() const override;
        virtual TextureHandle depthTexture() const override;

        virtual bool resize(const Vector2i &size) override;
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