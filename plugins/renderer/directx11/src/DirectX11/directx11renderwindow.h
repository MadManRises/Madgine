#pragma once

#include "directx11rendertarget.h"


#include "Meta/math/atlas2.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderWindow : DirectX11RenderTarget {
        DirectX11RenderWindow(DirectX11RenderContext *context, Window::OSWindow *w);
        ~DirectX11RenderWindow();

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

		virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;
        virtual size_t textureCount() const override;
                virtual TextureDescriptor depthTexture() const override;

		virtual bool resizeImpl(const Vector2i &size) override;
		virtual Vector2i size() const override;

    private:

		Window::OSWindow *mWindow;        

        ReleasePtr<IDXGISwapChain> mSwapChain;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11RenderWindow)