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
    
    Threading::WorkgroupLocal<DirectX11RenderContext *> sSingleton = nullptr;

    ID3D11DeviceContext *GetDeviceContext()
    {
        return sDeviceContext;
    }

    ID3D11Device *GetDevice()
    {
        return sDevice;
    }

    struct ConstantValues {
        Vector3 mPos { 0, 0, 0 };
        float mW = 1;
        Vector2 mPos2 { 0, 0 };
        Vector3 mNormal { 0, 0, 0 };
        Vector4 mColor { 1, 1, 1, 1 };
        Vector2 mUV { 0, 0 };
        int mBoneIndices[4] { 0, 0, 0, 0 };
        float mBoneWeights[4] { 0.0f, 0.0f, 0.0f, 0.0f };
    };

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

        sDeviceContext->QueryInterface(IID_PPV_ARGS(&mAnnotator));

        DX11_CHECK(hr);

        ConstantValues values;
        mConstantBuffer.setData({ &values, sizeof(values) });
        mConstantBuffer.bindVertex(0, 2);
    }

    DirectX11RenderContext::~DirectX11RenderContext()
    {
        if (mAnnotator) {
            mAnnotator->Release();
            mAnnotator = nullptr;
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

    void DirectX11RenderContext::pushAnnotation(const char *tag)
    {
        mAnnotator->BeginEvent(StringUtil::toWString(tag).c_str());
    }

    void DirectX11RenderContext::popAnnotation()
    {
        mAnnotator->EndEvent();
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
        1,
        2,
        0,
        0,
        0,
        0,
        0,
    };

    static constexpr UINT vConstantOffsets[] = {
        offsetof(ConstantValues, mPos),
        offsetof(ConstantValues, mW),
        offsetof(ConstantValues, mPos2),
        offsetof(ConstantValues, mNormal),
        offsetof(ConstantValues, mColor),
        offsetof(ConstantValues, mUV),
        offsetof(ConstantValues, mBoneIndices),
        offsetof(ConstantValues, mBoneWeights)
    };

    static constexpr DXGI_FORMAT vFormats[] = {
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_SINT,
        DXGI_FORMAT_R32G32B32A32_FLOAT
    };

    void DirectX11RenderContext::bindFormat(VertexFormat format, size_t instanceDataSize, ID3D10Blob *blob)
    {
        ReleasePtr<ID3D11InputLayout> &layout = mInputLayouts[format][instanceDataSize];

        if (!layout) {

#ifndef NDEBUG
#    define semantic(i) vSemantics[i]
#    define semanticIndex(i) vSemanticIndices[i]
#    define instanceSemantic "INSTANCEDATA"
#    define instanceSemanticIndex(i) (UINT) i
#else
#    define semantic(i) "TEXCOORD"
#    define semanticIndex(i) (UINT) i
#    define instanceSemantic "TEXCOORD"
#    define instanceSemanticIndex(i) (UINT)(VertexElements::size + i)
#endif

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexLayoutDesc;

            UINT offset = 0;
            for (size_t i = 0; i < VertexElements::size; ++i) {
                if (format.has(i)) {
                    vertexLayoutDesc.push_back({ semantic(i),
                        semanticIndex(i), vFormats[i], 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                    offset += sVertexElementSizes[i];
                } else {
                    vertexLayoutDesc.push_back({ semantic(i),
                        semanticIndex(i), vFormats[i], 2, vConstantOffsets[i], D3D11_INPUT_PER_VERTEX_DATA, 0 });
                }
            }

            assert(instanceDataSize % 16 == 0);
            for (size_t i = 0; i < instanceDataSize / 16; ++i) {
                vertexLayoutDesc.push_back({ instanceSemantic,
                    instanceSemanticIndex(i),
                    DXGI_FORMAT_R32G32B32A32_FLOAT,
                    1,
                    i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT,
                    D3D11_INPUT_PER_INSTANCE_DATA,
                    1 });
            }

            HRESULT hr = sDevice->CreateInputLayout(vertexLayoutDesc.data(), vertexLayoutDesc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &layout);
            DX11_CHECK(hr);
        }

        sDeviceContext->IASetInputLayout(layout);
    }

}
}
