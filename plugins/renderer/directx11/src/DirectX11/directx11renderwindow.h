#pragma once

#include "directx11rendertarget.h"

#include "util/directx11program.h"
#include "util/directx11texture.h"
#include "util/directx11vertexarray.h"
#include "util/directx11buffer.h"

#include "Modules/math/atlas2.h"

#include "imageloaderlib.h"
#include "imageloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderWindow : DirectX11RenderTarget {
		DirectX11RenderWindow(DirectX11RenderContext *context, Window::Window *w, DirectX11RenderWindow *sharedContext = nullptr);
        ~DirectX11RenderWindow();

        virtual void beginFrame() override;
        virtual void endFrame() override;

		Texture *texture() const override;

		virtual bool resize(const Vector2i &size) override;
		virtual Vector2i size() const override;

    private:

		Window::Window *mWindow;
        bool mReusedContext;

        IDXGISwapChain *mSwapChain;
    };

}
}