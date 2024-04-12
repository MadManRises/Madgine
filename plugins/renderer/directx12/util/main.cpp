
#include <assert.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <comdef.h>

#include "../src/DirectX12/d3dx12.h"

static void log(std::string_view s)
{
    static std::ofstream fOut { "dx12drivertest.log" };

    std::cout << s << std::endl;
    fOut << s << std::endl;
}


inline void dx12Check(const char *file, size_t line, HRESULT result = 0)
{
    if (FAILED(result)) {
        _com_error error { result };
        std::stringstream ss;
        ss << "DX12-Error (" << result << "): " << error.ErrorMessage() << "\n";
        ss << " at " << file << "(" << line << ")";
        log(ss.str());
        std::terminate();
    }
}

#define DX12_CHECK(...) dx12Check(__FILE__, __LINE__, __VA_ARGS__)


struct ReleaseDeleter {
    template <typename T>
    void operator()(T *ptr)
    {
        ptr->Release();
    }
};

template <typename T>
struct ReleasePtr : std::unique_ptr<T, ReleaseDeleter> {
    using std::unique_ptr<T, ReleaseDeleter>::unique_ptr;
    ReleasePtr(const ReleasePtr &other)
        : std::unique_ptr<T, ReleaseDeleter> { other.get() }
    {
        this->get()->AddRef();
    }
    ReleasePtr(ReleasePtr &&) = default;
    template <typename U>
    ReleasePtr(ReleasePtr<U> &&other)
        : std::unique_ptr<T, ReleaseDeleter> { std::move(other) }
    {
    }

    ReleasePtr &operator=(ReleasePtr &&) = default;

    T **operator&()
    {
        assert(!*this);
        return reinterpret_cast<T **>(this);
    }

    T *const *operator&() const
    {
        return reinterpret_cast<T *const *>(this);
    }

    operator T *() const
    {
        return this->get();
    }
};


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

void logProperties(ID3D12Device* device) {
    D3D12_FEATURE_DATA_D3D12_OPTIONS options;

    HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
    DX12_CHECK(hr);

    log("Device Properties:");
    log("    ResourceHeapTier: " + std::to_string(options.ResourceHeapTier));
    log("");
}

void testCreateHeap(ID3D12Device* device) {
    log("Testing CreateHeap");

    for (size_t s = 8388608; s <= 8388608; s *= 2) {
        std::stringstream ss;
        ss << "Size: " << s << "...\n";

        ss << "  Flags: None... ";

        CD3DX12_HEAP_DESC heapDesc = CD3DX12_HEAP_DESC { s, D3D12_HEAP_TYPE_DEFAULT };

        ReleasePtr<ID3D12Heap> heap;

        HRESULT hr = device->CreateHeap(
            &heapDesc,
            IID_PPV_ARGS(&heap));
        if (FAILED(hr)) {
            ss << "Failed (" << std::hex << hr << "): \n";
            _com_error error { hr };
            ss << error.ErrorMessage();
        } else {
            ss << "Success!";
        }
        heap.reset();

        ss << "\n  Flags: Deny Textures... ";

        heapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;

        hr = device->CreateHeap(
            &heapDesc,
            IID_PPV_ARGS(&heap));
        if (FAILED(hr)) {
            ss << "Failed (" << std::hex << hr << "): \n";
            _com_error error { hr };
            ss << error.ErrorMessage();
        } else {
            ss << "Success!";
        }
        heap.reset();



        log(ss.str());
    }
}


int main() {

    ReleasePtr<IDXGIFactory4> factory;

    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    DX12_CHECK(hr);

    ReleasePtr<IDXGIAdapter1> hardwareAdapter = GetHardwareAdapter(factory);

    ReleasePtr<ID3D12Device> device;

    log("Creating Device...");
    hr = D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
    DX12_CHECK(hr);
    log("Success\n");

    logProperties(device);

    testCreateHeap(device);
}