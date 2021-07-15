#include "directx12lib.h"

#include "directx12renderwindow.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "directx12rendercontext.h"
#include "directx12rendertexture.h"

UNIQUECOMPONENT(Engine::Render::DirectX12RenderContext)

METATABLE_BEGIN(Engine::Render::DirectX12RenderContext)
METATABLE_END(Engine::Render::DirectX12RenderContext)

namespace Engine {
namespace Render {

    static void GetHardwareAdapter(IDXGIFactory4 *pFactory, IDXGIAdapter1 **ppAdapter)
    {
        *ppAdapter = nullptr;
        for (UINT adapterIndex = 0;; ++adapterIndex) {
            IDXGIAdapter1 *pAdapter = nullptr;
            if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter)) {
                // No more adapters to enumerate.
                break;
            }

            // Check to see if the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                *ppAdapter = pAdapter;
                return;
            }
            pAdapter->Release();
        }
    }

    THREADLOCAL(ID3D12Device *)
    sDevice = nullptr;

    THREADLOCAL(DirectX12RenderContext *)
    sSingleton = nullptr;

    ID3D12Device *GetDevice()
    {
        return sDevice;
    }

    DirectX12RenderContext::DirectX12RenderContext(Threading::TaskQueue *queue)
        : UniqueComponent(queue)
    {

        assert(!sSingleton);

        sSingleton = this;

        HRESULT hr;

        ID3D12Debug *debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            debugController->Release();
        }

        assert(sDevice == nullptr);

        hr = CreateDXGIFactory1(IID_PPV_ARGS(&mFactory));
        DX12_CHECK(hr);

        IDXGIAdapter1 *hardwareAdapter;
        GetHardwareAdapter(mFactory, &hardwareAdapter);

        hr = D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&sDevice));
        DX12_CHECK(hr);
        hardwareAdapter->Release();

        mDescriptorHeap = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        mRenderTargetDescriptorHeap = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        mDepthStencilDescriptorHeap = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        mConstantBufferHeap = { &mDescriptorHeap, 64 * 1024 };

        D3D12_COMMAND_QUEUE_DESC queueDesc {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        hr = sDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
        DX12_CHECK(hr);

        mCommandList.mAllocator = fetchCommandAllocator();

        ID3D12DescriptorHeap *heap = mDescriptorHeap.resource();

        hr = sDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandList.mAllocator, nullptr, IID_PPV_ARGS(&mCommandList.mList));
        DX12_CHECK(hr);

        mCommandList.mList->SetDescriptorHeaps(1, &heap);

        mTempCommandList.mAllocator = fetchCommandAllocator();

        hr = sDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mTempCommandList.mAllocator, nullptr, IID_PPV_ARGS(&mTempCommandList.mList));
        DX12_CHECK(hr);

        mTempCommandList.mList->SetDescriptorHeaps(1, &heap);

        hr = sDevice->CreateFence(mLastCompletedFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        DX12_CHECK(hr);

        mNextFenceValue = mLastCompletedFenceValue + 1;

        mFenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        createRootSignature();
    }

    DirectX12RenderContext::~DirectX12RenderContext()
    {
        if (sDevice) {
            sDevice->Release();
            sDevice = nullptr;
        }

        if (mFactory) {
            mFactory->Release();
            mFactory = nullptr;
        }

        assert(sSingleton == this);
        sSingleton = nullptr;
    }

    std::unique_ptr<RenderTarget> DirectX12RenderContext::createRenderTexture(const Vector2i &size, const RenderTextureConfig &config)
    {
        return std::make_unique<DirectX12RenderTexture>(this, size, config);
    }

    void DirectX12RenderContext::beginFrame()
    {
        mCommandList.mList->SetGraphicsRootSignature(mRootSignature);
    }

    void DirectX12RenderContext::endFrame()
    {
        ExecuteCommandList(mCommandList);
    }

    void DirectX12RenderContext::waitForGPU()
    {
        waitForFence(mNextFenceValue - 1);
    }

    void DirectX12RenderContext::waitForFence(uint64_t fenceValue)
    {
        if (isFenceComplete(fenceValue))
            return;

        // Wait until the fence has been processed.
        HRESULT hr = mFence->SetEventOnCompletion(fenceValue, mFenceEvent);
        DX12_CHECK(hr);
        WaitForSingleObject(mFenceEvent, INFINITE);

        mLastCompletedFenceValue = fenceValue;
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
        CD3DX12_ROOT_PARAMETER rootParameters[5];

        rootParameters[0].InitAsConstantBufferView(0);
        rootParameters[1].InitAsConstantBufferView(1);
        rootParameters[2].InitAsConstantBufferView(2);
        rootParameters[3].InitAsShaderResourceView(0);
        rootParameters[4].InitAsShaderResourceView(1);

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        rootSignatureDesc.Init(5, rootParameters, 0, nullptr, rootSignatureFlags);

        ID3DBlob *signature = nullptr;
        ID3DBlob *error = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        DX12_CHECK(hr);
        hr = sDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
        DX12_CHECK(hr);
        if (signature) {
            signature->Release();
        }
        if (error) {
            error->Release();
        }
    }

    void DirectX12RenderContext::ExecuteCommandList(DirectX12CommandList &list, std::function<void()> dtor)
    {
        HRESULT hr = list.mList->Close();
        DX12_CHECK(hr);        

        ID3D12CommandList *cmdList = list.mList;
        mCommandQueue->ExecuteCommandLists(1, &cmdList);
        DX12_CHECK();

        mCommandQueue->Signal(mFence, mNextFenceValue);
        ++mNextFenceValue;

        list.mAllocator = fetchCommandAllocator(list.mAllocator, std::move(dtor));

        hr = list.mList->Reset(list.mAllocator, nullptr);
        DX12_CHECK(hr);

        ID3D12DescriptorHeap *heap = mDescriptorHeap.resource();
        list.mList->SetDescriptorHeaps(1, &heap);
    }

    ID3D12CommandAllocator *DirectX12RenderContext::fetchCommandAllocator(ID3D12CommandAllocator *discardAllocator, std::function<void()> dtor)
    {
        if (discardAllocator)
            mAllocatorPool.emplace_back(mNextFenceValue, discardAllocator, std::move(dtor));

        if (!mAllocatorPool.empty()) {
            auto &[fenceValue, allocator, dtor] = mAllocatorPool.front();

            if (fenceValue <= mLastCompletedFenceValue) {
                if (dtor)
                    dtor();
                ID3D12CommandAllocator *alloc = allocator;
                mAllocatorPool.erase(mAllocatorPool.begin());
                HRESULT hr = alloc->Reset();
                return alloc;
            }
        }

        ID3D12CommandAllocator *alloc;
        HRESULT hr = sDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc));
        DX12_CHECK(hr);
        return alloc;
    }

    std::unique_ptr<RenderTarget> DirectX12RenderContext::createRenderWindow(Window::OSWindow *w)
    {
        checkThread();

        return std::make_unique<DirectX12RenderWindow>(this, w);
    }

}
}
