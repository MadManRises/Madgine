#include "directx11lib.h"

#include "directx11renderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
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
            HRESULT hr;

            UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

            // These are the feature levels that we will accept.
            D3D_FEATURE_LEVEL featureLevels[] = {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };

            // This will be the feature level that
            // is used to create our device and swap chain.
            D3D_FEATURE_LEVEL featureLevel;

            assert(sDeviceContext == nullptr);

            hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
                D3D11_SDK_VERSION, &sDevice, &featureLevel,
                &sDeviceContext);

            if (hr == E_INVALIDARG) {
                hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                    nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
                    D3D11_SDK_VERSION, &sDevice, &featureLevel,
                    &sDeviceContext);
            }

            DX11_CHECK(hr);
        }

        DirectX11RenderContext::~DirectX11RenderContext()
        {
            if (sDevice) {
                sDevice->Release();
                sDevice = nullptr;
            }
        }

        std::unique_ptr<RenderTarget> DirectX11RenderContext::createRenderTexture(const Vector2i &size, const RenderTextureConfig &config)
        {
            return std::make_unique<DirectX11RenderTexture>(this, size, config);
        }

        std::unique_ptr<RenderTarget> DirectX11RenderContext::createRenderWindow(Window::OSWindow *w)
        {
            checkThread();

            return std::make_unique<DirectX11RenderWindow>(this, w);
        }

    }
}
