#pragma once

#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/render/rendercontext.h"

#include "Modules/threading/threadlocal.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Render {

    extern THREADLOCAL(ID3D11DeviceContext *) sDeviceContext;
    extern THREADLOCAL(ID3D11Device *) sDevice;
    extern THREADLOCAL(ID3DUserDefinedAnnotation *) sAnnotator;

    MADGINE_DIRECTX11_EXPORT ID3D11DeviceContext *GetDeviceContext();
    MADGINE_DIRECTX11_EXPORT ID3D11Device *GetDevice();

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderContext : public RenderContextComponent<DirectX11RenderContext> {
        DirectX11RenderContext(Threading::TaskQueue *queue);
        ~DirectX11RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;
    };

}
}

RegisterType(Engine::Render::DirectX11RenderContext);