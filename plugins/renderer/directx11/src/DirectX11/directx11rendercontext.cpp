#include "directx11lib.h"

#include "directx11renderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "directx11rendercontext.h"
#include "directx11rendertexture.h"

#include "directx11meshloader.h"
#include "directx11pipelineloader.h"
#include "directx11textureloader.h"

UNIQUECOMPONENT(Engine::Render::DirectX11RenderContext)

METATABLE_BEGIN(Engine::Render::DirectX11RenderContext)
METATABLE_END(Engine::Render::DirectX11RenderContext)

namespace Engine {
namespace Render {

    THREADLOCAL(ID3D11Device *)
    sDevice = nullptr;
    THREADLOCAL(ID3D11DeviceContext *)
    sDeviceContext = nullptr;
    THREADLOCAL(ID3DUserDefinedAnnotation *)
    sAnnotator = nullptr;

    Threading::WorkgroupLocal<DirectX11RenderContext *> sSingleton = nullptr;

    ID3D11DeviceContext *GetDeviceContext()
    {
        return sDeviceContext;
    }

    ID3D11Device *GetDevice()
    {
        return sDevice;
    }

    DirectX11RenderContext::DirectX11RenderContext(Threading::TaskQueue *queue)
        : Component(queue)
    {
        assert(!sSingleton);

        sSingleton = this;

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

        sDeviceContext->QueryInterface(IID_PPV_ARGS(&sAnnotator));

        DX11_CHECK(hr);
    }

    DirectX11RenderContext::~DirectX11RenderContext()
    {
        if (sAnnotator) {
            sAnnotator->Release();
            sAnnotator = nullptr;
        }
        if (sDeviceContext) {
            sDeviceContext->Release();
            sDeviceContext = nullptr;
        }
        if (sDevice) {
            sDevice->Release();
            sDevice = nullptr;
        }

        assert(sSingleton == this);
        sSingleton = nullptr;
    }

    DirectX11RenderContext &DirectX11RenderContext::getSingleton()
    {
        return *sSingleton;
    }

    std::unique_ptr<RenderTarget> DirectX11RenderContext::createRenderTexture(const Vector2i &size, const RenderTextureConfig &config)
    {
        return std::make_unique<DirectX11RenderTexture>(this, size, config);
    }

    std::unique_ptr<RenderTarget> DirectX11RenderContext::createRenderWindow(Window::OSWindow *w, size_t samples)
    {
        checkThread();

        return std::make_unique<DirectX11RenderWindow>(this, w);
    }

    void DirectX11RenderContext::unloadAllResources()
    {
        RenderContext::unloadAllResources();

        for (std::pair<const std::string, DirectX11PipelineLoader::Resource> &res : DirectX11PipelineLoader::getSingleton()) {
            res.second.forceUnload();
        }

        for (std::pair<const std::string, DirectX11TextureLoader::Resource> &res : DirectX11TextureLoader::getSingleton()) {
            res.second.forceUnload();
        }

        for (std::pair<const std::string, DirectX11MeshLoader::Resource> &res : DirectX11MeshLoader::getSingleton()) {
            res.second.forceUnload();
        }
    }

    bool DirectX11RenderContext::supportsMultisampling() const
    {
        return true;
    }

    static constexpr const char *vSemantics[] = {
        "POSITION",
        "POSITION",
        "POSITION",
        "NORMAL",
        "COLOR",
        "TEXCOORD",
        "BONEINDICES",
        "WEIGHTS"
    };

    static constexpr unsigned int vSemanticIndices[] = {
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        0,
    };

    static constexpr DXGI_FORMAT vFormats[] = {
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_SINT,
        DXGI_FORMAT_R32G32B32A32_FLOAT
    };

    void DirectX11RenderContext::ensureFormat(VertexFormat format, size_t instanceDataSize, ID3DBlob *blob)
    {
        ReleasePtr<ID3D11InputLayout> &layout = mInputLayouts[format][instanceDataSize];

        if (!layout) {
            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexLayoutDesc;

            UINT offset = 0;
            for (size_t i = 0; i < VertexElements::size; ++i) {
                if (format.has(i)) {
                    vertexLayoutDesc.push_back({ vSemantics[i],
                        vSemanticIndices[i], vFormats[i], 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                    offset += sVertexElementSizes[i];
                }
            }

            if (instanceDataSize > 0) {
                assert(instanceDataSize % 16 == 0);
                for (size_t i = 0; i < instanceDataSize / 16; ++i) {
                    vertexLayoutDesc.push_back({ "INSTANCEDATA",
                        static_cast<UINT>(i),
                        DXGI_FORMAT_R32G32B32A32_FLOAT,
                        1,
                        i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT,
                        D3D11_INPUT_PER_INSTANCE_DATA,
                        1 });
                }
            }
            HRESULT hr = sDevice->CreateInputLayout(vertexLayoutDesc.data(), vertexLayoutDesc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &layout);
            DX11_CHECK(hr);
        }
    }

    void DirectX11RenderContext::bindFormat(VertexFormat format, size_t instanceDataSize)
    {
        sDeviceContext->IASetInputLayout(mInputLayouts[format].at(instanceDataSize));
    }

}
}
