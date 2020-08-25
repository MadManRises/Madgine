#include "directx11lib.h"

#include "directx11renderwindow.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "directx11rendercontext.h"
#include "directx11rendertexture.h"



    UNIQUECOMPONENT(Engine::Render::DirectX11RenderContext)

        METATABLE_BEGIN(Engine::Render::DirectX11RenderContext)
            METATABLE_END(Engine::Render::DirectX11RenderContext)

                namespace Engine
{
    namespace Render {

        THREADLOCAL(ID3D11Device *) sDevice = nullptr;
        THREADLOCAL(ID3D11DeviceContext *) sDeviceContext = nullptr;

        ID3D11DeviceContext *GetDeviceContext()
        {
            return sDeviceContext;
        }

        ID3D11Device *GetDevice()
        {
            return sDevice;
        }

        DirectX11RenderContext::DirectX11RenderContext(Threading::TaskQueue *queue)
            : UniqueComponent(queue)
        {
        }

        DirectX11RenderContext::~DirectX11RenderContext()
        {
        }

        std::unique_ptr<RenderTarget> DirectX11RenderContext::createRenderTexture(const Vector2i &size)
        {
            return std::make_unique<DirectX11RenderTexture>(this, size);
        }

        std::unique_ptr<RenderTarget> DirectX11RenderContext::createRenderWindow(Window::OSWindow *w)
        {
            checkThread();

            DirectX11RenderWindow *sharedContext = nullptr;
            for (RenderTarget *target : mRenderTargets) {
                sharedContext = dynamic_cast<DirectX11RenderWindow *>(target);
                if (sharedContext)
                    break;
            }

            return std::make_unique<DirectX11RenderWindow>(this, w, sharedContext);
        }

    }
}
