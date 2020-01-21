#pragma once

#include "Madgine/render/rendercontextcollector.h"

#include "Modules/threading/threadlocal.h"

namespace Engine {
namespace Render {

    extern THREADLOCAL(ID3D11DeviceContext *) sDeviceContext;
    extern THREADLOCAL(ID3D11Device *) sDevice;

    MADGINE_DIRECTX11_EXPORT ID3D11DeviceContext *GetDeviceContext();
    MADGINE_DIRECTX11_EXPORT ID3D11Device *GetDevice();

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderContext : public RenderContextComponent<DirectX11RenderContext> {
        DirectX11RenderContext(Threading::TaskQueue *queue);
        ~DirectX11RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::Window *w) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }) override;
    };

}
}