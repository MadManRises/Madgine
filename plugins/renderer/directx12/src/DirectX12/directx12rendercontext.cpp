#include "directx12lib.h"

#include "directx12renderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "directx12rendercontext.h"
#include "directx12rendertexture.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/threading/workgroupstorage.h"

#include "directx12meshloader.h"
#include "directx12pipelineloader.h"
#include "directx12textureloader.h"

#include "Madgine/render/constantvalues.h"

UNIQUECOMPONENT(Engine::Render::DirectX12RenderContext)

METATABLE_BEGIN(Engine::Render::DirectX12RenderContext)
METATABLE_END(Engine::Render::DirectX12RenderContext)

namespace Engine {
namespace Render {

    static void __stdcall dxDebugOutput(D3D12_MESSAGE_CATEGORY category,
        D3D12_MESSAGE_SEVERITY severity,
        D3D12_MESSAGE_ID id,
        LPCSTR message,
        void *context)
    {

        Util::MessageType lvl;
        switch (severity) {
        case D3D12_MESSAGE_SEVERITY_ERROR:
            lvl = Util::MessageType::ERROR_TYPE;
            break;
        case D3D12_MESSAGE_SEVERITY_WARNING:
            lvl = Util::MessageType::WARNING_TYPE;
            break;
        case D3D12_MESSAGE_SEVERITY_INFO:
            lvl = Util::MessageType::INFO_TYPE;
            break;
        case D3D12_MESSAGE_SEVERITY_MESSAGE:
            lvl = Util::MessageType::DEBUG_TYPE;
            break;
        }

        Util::LogDummy cout(lvl);
        cout << "Debug message (" << id << "): " << message << "\n";

        /* switch (source) {
        case GL_DEBUG_SOURCE_API:
            cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            cout << "Source: Other";
            break;
        }
        cout << "\n";

        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            cout << "Type: Other";
            break;
        }
        cout << "\n";*/
    }

    static ReleasePtr<IDXGIAdapter1> GetHardwareAdapter(IDXGIFactory4 *pFactory)
    {
        for (UINT adapterIndex = 0;; ++adapterIndex) {
            ReleasePtr<IDXGIAdapter1> pAdapter;
            if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter)) {
                // No more adapters to enumerate.
                break;
            }

            // Check to see if the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                return pAdapter;
            }
        }
        return {};
    }

    Threading::WorkgroupLocal<ReleasePtr<ID3D12Device>> sDevice;

    Threading::WorkgroupLocal<DirectX12RenderContext *> sSingleton = nullptr;

    ID3D12Device *GetDevice()
    {
        return *sDevice;
    }

    DirectX12RenderContext::DirectX12RenderContext(Threading::TaskQueue *queue)
        : Component(queue)
    {

        assert(!sSingleton);

        sSingleton = this;

        HRESULT hr;

        {
            ReleasePtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
            }
        }

        assert(*sDevice == nullptr);

        hr = CreateDXGIFactory1(IID_PPV_ARGS(&mFactory));
        DX12_CHECK(hr);

        ReleasePtr<IDXGIAdapter1> hardwareAdapter = GetHardwareAdapter(mFactory);

        hr = D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&*sDevice));
        DX12_CHECK(hr);

        {
            ReleasePtr<ID3D12InfoQueue1> infoQueue;
            hr = GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue));

            if (SUCCEEDED(hr)) {

                hr = infoQueue->RegisterMessageCallback(
                    &dxDebugOutput,
                    D3D12_MESSAGE_CALLBACK_FLAG_NONE,
                    nullptr,
                    nullptr);
                DX12_CHECK(hr);
            }
        }

        mDescriptorHeap = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        mRenderTargetDescriptorHeap = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        mDepthStencilDescriptorHeap = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        mConstantBufferHeap = { 16 * 1024 * 1024 };

        D3D12_COMMAND_QUEUE_DESC queueDesc {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        hr = GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
        DX12_CHECK(hr);

        mLastCompletedFenceValue = 5;
        hr = GetDevice()->CreateFence(mLastCompletedFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        DX12_CHECK(hr);

        mNextFenceValue = mLastCompletedFenceValue + 1;

        createRootSignature();

        ConstantValues values;
        mConstantBuffer.setData({ &values, sizeof(values) });
    }

    DirectX12RenderContext::~DirectX12RenderContext()
    {
        mConstantBuffer.reset();

        (*sDevice).reset();

        assert(sSingleton == this);
        sSingleton = nullptr;
    }

    std::unique_ptr<RenderTarget> DirectX12RenderContext::createRenderTexture(const Vector2i &size, const RenderTextureConfig &config)
    {
        return std::make_unique<DirectX12RenderTexture>(this, size, config);
    }

    bool DirectX12RenderContext::beginFrame()
    {
        if (!isFenceComplete(mNextFenceValue - 2))
            return false;
        return RenderContext::beginFrame();
    }

    void DirectX12RenderContext::endFrame()
    {
        RenderContext::endFrame();

        mCommandQueue->Signal(mFence, mNextFenceValue);
        ++mNextFenceValue;
    }

    uint64_t DirectX12RenderContext::currentFence()
    {
        return mNextFenceValue - 1;
    }

    bool DirectX12RenderContext::isFenceComplete(uint64_t fenceValue)
    {
        // if it's greater than last seen fence value
        // check fence for latest completed value
        if (fenceValue > mLastCompletedFenceValue)
            mLastCompletedFenceValue = std::max(mLastCompletedFenceValue, mFence->GetCompletedValue());

        return fenceValue <= mLastCompletedFenceValue;
    }

    DirectX12RenderContext &DirectX12RenderContext::getSingleton()
    {
        return *sSingleton;
    }

    void DirectX12RenderContext::createRootSignature()
    {
        CD3DX12_ROOT_PARAMETER rootParameters[9];

        rootParameters[0].InitAsConstantBufferView(0);
        rootParameters[1].InitAsConstantBufferView(1);
        rootParameters[2].InitAsConstantBufferView(2);
        rootParameters[3].InitAsConstantBufferView(3);

        CD3DX12_DESCRIPTOR_RANGE range[5];

        for (size_t i = 0; i < 5; ++i) {
            range[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
            rootParameters[4 + i].InitAsDescriptorTable(1, range + i);
        }

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        D3D12_STATIC_SAMPLER_DESC samplerDesc[2];
        ZeroMemory(samplerDesc, sizeof(samplerDesc));

        samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[0].MipLODBias = 0.0f;
        samplerDesc[0].MaxAnisotropy = 1;
        samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[0].MinLOD = 0;
        samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;

        samplerDesc[1] = samplerDesc[0];

        samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[1].ShaderRegister = 1;

        rootSignatureDesc.Init(9, rootParameters, 2, samplerDesc, rootSignatureFlags);

        ReleasePtr<ID3DBlob> signature;
        ReleasePtr<ID3DBlob> error;
        HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        DX12_CHECK(hr);
        hr = GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
        DX12_CHECK(hr);
    }

    DirectX12CommandList DirectX12RenderContext::fetchCommandList(std::string_view name)
    {
        ReleasePtr<ID3D12CommandAllocator> alloc;

        if (!mAllocatorPool.empty()) {
            auto &[fenceValue, allocator, discard] = mAllocatorPool.front();

            if (isFenceComplete(fenceValue)) {
                alloc = std::move(allocator);
                mAllocatorPool.erase(mAllocatorPool.begin());
                HRESULT hr = alloc->Reset();
                DX12_CHECK(hr);
            }
        }

        if (!alloc) {
            HRESULT hr = GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc));
            DX12_CHECK(hr);
        }

        ReleasePtr<ID3D12GraphicsCommandList> list;

        if (!mCommandListPool.empty()) {
            list = std::move(mCommandListPool.back());
            mCommandListPool.pop_back();
            HRESULT hr = list->Reset(alloc, nullptr);
            DX12_CHECK(hr);
        } else {
            HRESULT hr = GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc, nullptr, IID_PPV_ARGS(&list));
            DX12_CHECK(hr);
        }

        list->SetName(StringUtil::toWString(name).c_str());

        ID3D12DescriptorHeap *heap = mDescriptorHeap.resource();
        list->SetDescriptorHeaps(1, &heap);

        return { std::move(list), std::move(alloc) };
    }

    void DirectX12RenderContext::ExecuteCommandList(ReleasePtr<ID3D12GraphicsCommandList> list, ReleasePtr<ID3D12CommandAllocator> allocator, std::vector<ReleasePtr<ID3D12Pageable>> discardResources)
    {
        HRESULT hr = list->Close();
        DX12_CHECK(hr);

        ID3D12CommandList *cList = list;
        mCommandQueue->ExecuteCommandLists(1, &cList);
        DX12_CHECK();

        if (allocator || !discardResources.empty())
            mAllocatorPool.emplace_back(mNextFenceValue, std::move(allocator), std::move(discardResources));

        mCommandListPool.push_back(std::move(list));
    }

    std::unique_ptr<RenderTarget> DirectX12RenderContext::createRenderWindow(Window::OSWindow *w, size_t samples)
    {
        checkThread();

        return std::make_unique<DirectX12RenderWindow>(this, w, samples);
    }

    Threading::Task<void> DirectX12RenderContext::unloadAllResources()
    {
        co_await RenderContext::unloadAllResources();

        for (std::pair<const std::string, DirectX12PipelineLoader::Resource> &res : DirectX12PipelineLoader::getSingleton()) {
            co_await res.second.forceUnload();
        }

        for (std::pair<const std::string, DirectX12TextureLoader::Resource> &res : DirectX12TextureLoader::getSingleton()) {
            co_await res.second.forceUnload();
        }

        for (std::pair<const std::string, DirectX12MeshLoader::Resource> &res : DirectX12MeshLoader::getSingleton()) {
            co_await res.second.forceUnload();
        }
    }

    bool DirectX12RenderContext::supportsMultisampling() const
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
        1,
        2,
        0,
        0,
        0,
        0,
        0,
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

    std::vector<D3D12_INPUT_ELEMENT_DESC> Engine::Render::DirectX12RenderContext::createVertexLayout(VertexFormat format, size_t instanceDataSize)
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayoutDesc;

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

        UINT offset = 0;
        for (UINT i = 0; i < VertexElements::size; ++i) {
            if (format.has(i)) {
                vertexLayoutDesc.push_back({ semantic(i),
                    semanticIndex(i), vFormats[i], 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
                offset += sVertexElementSizes[i];
            } else {
                vertexLayoutDesc.push_back({ semantic(i),
                    semanticIndex(i), vFormats[i], 2, vConstantOffsets[i], D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
            }
        }

        assert(instanceDataSize % 16 == 0);
        for (size_t i = 0; i < instanceDataSize / 16; ++i) {
            vertexLayoutDesc.push_back({ instanceSemantic,
                instanceSemanticIndex(i),
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                1,
                i == 0 ? 0 : D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
                1 });
        }

        return vertexLayoutDesc;
    }

}
}
