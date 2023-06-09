#include "../directx12lib.h"

#include "directx12texture.h"

#include "../directx12rendercontext.h"

#include "Generic/align.h"

namespace Engine {
namespace Render {

    DirectX12Texture::DirectX12Texture(TextureType type, bool isRenderTarget, DataFormat format, size_t width, size_t height, size_t samples, const ByteBuffer &data)
        : Texture(type, format, { static_cast<int>(width), static_cast<int>(height) })
        , mIsRenderTarget(isRenderTarget)
        , mSamples(samples)
    {
        DXGI_FORMAT xFormat;
        D3D12_SRV_DIMENSION dimension;
        size_t byteCount;
        switch (format) {
        case FORMAT_RGBA8:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            byteCount = 4;
            break;
        case FORMAT_RGBA16F:
            xFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
            byteCount = 8;
            break;
        default:
            std::terminate();
        }

        assert(samples == 1 || type == TextureType_2DMultiSample);

        switch (type) {
        case TextureType_2D:
        case TextureType_2DMultiSample: {
            D3D12_RESOURCE_DESC textureDesc {};

            textureDesc.DepthOrArraySize = 1;
            textureDesc.Format = xFormat;
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.MipLevels = 1;
            textureDesc.SampleDesc.Count = samples;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Flags = isRenderTarget ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            D3D12_CLEAR_VALUE clear {};
            clear.Format = xFormat;
            clear.Color[0] = 0.2f;
            clear.Color[1] = 0.3f;
            clear.Color[2] = 0.3f;
            clear.Color[3] = 1.0f;

            auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            HRESULT hr = GetDevice()->CreateCommittedResource(
                &heapDesc,
                D3D12_HEAP_FLAG_NONE,
                &textureDesc,
                data.mData ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                isRenderTarget ? &clear : nullptr,
                IID_PPV_ARGS(&mResource));
            DX12_CHECK(hr);
            if (data.mData) {
                assert(!isRenderTarget);
                const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mResource, 0, 1);

                heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
                ReleasePtr<ID3D12Resource> uploadHeap;
                hr = GetDevice()->CreateCommittedResource(
                    &heapDesc,
                    D3D12_HEAP_FLAG_NONE,
                    &resourceDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&uploadHeap));
                DX12_CHECK(hr);

                D3D12_SUBRESOURCE_DATA subResourceDesc;
                subResourceDesc.pData = data.mData;
                subResourceDesc.RowPitch = width * byteCount;
                subResourceDesc.SlicePitch = subResourceDesc.RowPitch * height;

                DirectX12CommandList list = DirectX12RenderContext::getSingleton().fetchCommandList("upload");

                UpdateSubresources(list, mResource, uploadHeap, 0, 0, 1, &subResourceDesc);
                list.Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                list.attachResource(std::move(uploadHeap));
            }
            dimension = type == TextureType_2DMultiSample ? D3D12_SRV_DIMENSION_TEXTURE2DMS : D3D12_SRV_DIMENSION_TEXTURE2D;

            break;
        }
        default:
            std::terminate();
        }

        D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc {};
        shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shaderResourceViewDesc.Format = xFormat;
        shaderResourceViewDesc.ViewDimension = dimension;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        OffsetPtr handle = DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();
        mTextureHandle = DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(handle).ptr;
        GetDevice()->CreateShaderResourceView(mResource, &shaderResourceViewDesc, DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(handle));
        DX12_CHECK();
    }

    DirectX12Texture::DirectX12Texture(TextureType type, bool isRenderTarget, DataFormat format, size_t samples)
        : Texture(type, format)
        , mIsRenderTarget(isRenderTarget)
        , mSamples(samples)
    {
    }

    DirectX12Texture::DirectX12Texture(DirectX12Texture &&other)
        : Texture(std::move(other))
        , mResource(std::exchange(other.mResource, nullptr))
        , mIsRenderTarget(std::exchange(other.mIsRenderTarget, false))
        , mSamples(std::exchange(other.mSamples, 0))
    {
    }

    DirectX12Texture::~DirectX12Texture()
    {
        reset();
    }

    DirectX12Texture &DirectX12Texture::operator=(DirectX12Texture &&other)
    {
        Texture::operator=(std::move(other));
        std::swap(mResource, other.mResource);
        std::swap(mIsRenderTarget, other.mIsRenderTarget);
        std::swap(mSamples, other.mSamples);
        return *this;
    }

    void DirectX12Texture::reset()
    {
        if (mTextureHandle) {
            mResource.reset();
            DirectX12RenderContext::getSingleton().mDescriptorHeap.deallocate(DirectX12RenderContext::getSingleton().mDescriptorHeap.fromGpuHandle({ mTextureHandle }));
            mTextureHandle = 0;
            mSamples = 0;
        }
    }

    void DirectX12Texture::setData(Vector2i size, const ByteBuffer &data)
    {
        *this = DirectX12Texture { mType, mIsRenderTarget, mFormat, static_cast<size_t>(size.x), static_cast<size_t>(size.y), mSamples, data };
    }

    void DirectX12Texture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        DXGI_FORMAT xFormat;
        D3D12_SRV_DIMENSION dimension;
        size_t byteCount;
        switch (mFormat) {
        case FORMAT_RGBA8:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            byteCount = 4;
            break;
        case FORMAT_RGBA16F:
            xFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
            byteCount = 8;
            break;
        default:
            std::terminate();
        }

        CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(xFormat, size.x, size.y);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
        UINT numRows;
        UINT64 rowSize;
        UINT64 totalSize;
        GetDevice()->GetCopyableFootprints(&resDesc, 0, 1, 0, &layout, &numRows, &rowSize, &totalSize);

        auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);
        ReleasePtr<ID3D12Resource> uploadHeap;
        HRESULT hr = GetDevice()->CreateCommittedResource(
            &heapDesc,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadHeap));
        DX12_CHECK(hr);

        D3D12_SUBRESOURCE_DATA subResourceDesc;
        subResourceDesc.pData = data.mData;
        subResourceDesc.RowPitch = size.x * byteCount;
        subResourceDesc.SlicePitch = subResourceDesc.RowPitch * size.y;

        BYTE *pData;
        hr = uploadHeap->Map(0, nullptr, reinterpret_cast<void **>(&pData));
        if (FAILED(hr)) {
            throw 0;
        }

        D3D12_MEMCPY_DEST DestData = { pData + layout.Offset, layout.Footprint.RowPitch, SIZE_T(layout.Footprint.RowPitch) * SIZE_T(numRows) };
        MemcpySubresource(&DestData, &subResourceDesc, rowSize, numRows, 1);
        uploadHeap->Unmap(0, nullptr);

        DirectX12CommandList list = DirectX12RenderContext::getSingleton().fetchCommandList("upload");

        list.Transition(mResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

        CD3DX12_TEXTURE_COPY_LOCATION Dst(mResource, 0);
        CD3DX12_TEXTURE_COPY_LOCATION Src(uploadHeap, layout);
        list->CopyTextureRegion(&Dst, offset.x, offset.y, 0, &Src, nullptr);

        list.Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        list.attachResource(std::move(uploadHeap));
    }

    DirectX12Texture::operator ID3D12Resource *() const
    {
        return mResource;
    }

    DirectX12Texture::operator ReleasePtr<ID3D12Resource>() const
    {
        return mResource;
    }

    void DirectX12Texture::setName(std::string_view name)
    {
        mResource->SetName(StringUtil::toWString(name).c_str());
    }
}
}