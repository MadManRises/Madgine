#include "../directx12lib.h"

#include "directx12texture.h"

#include "../directx12rendercontext.h"

#include "Generic/align.h"

namespace Engine {
namespace Render {

    DirectX12Texture::DirectX12Texture(TextureType type, bool isRenderTarget, TextureFormat format, size_t width, size_t height, size_t samples, const ByteBuffer &data)
        : Texture(type, format, { static_cast<int>(width), static_cast<int>(height) })
        , mIsRenderTarget(isRenderTarget)
        , mSamples(samples)
    {
        bool isDepthTarget = mFormat == FORMAT_D24;

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
        case FORMAT_RGBA8_SRGB:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            byteCount = 4;
            break;
        case FORMAT_D24:
            xFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            byteCount = 4;
            break;
        default:
            std::terminate();
        }

        assert(samples == 1 || type == TextureType_2DMultiSample);

        D3D12_RESOURCE_DESC textureDesc {};

        textureDesc.Format = xFormat;
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.SampleDesc.Count = samples;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Flags = isRenderTarget ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : isDepthTarget ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;

        D3D12_CLEAR_VALUE clear {};
        clear.Format = xFormat;
        if (isRenderTarget) {
            clear.Color[0] = 0.033f;
            clear.Color[1] = 0.073f;
            clear.Color[2] = 0.073f;
            clear.Color[3] = 1.0f;
        } else if (isDepthTarget) {
            clear.DepthStencil.Depth = 1.0f;
            clear.DepthStencil.Stencil = 0;
        }

        switch (type) {
        case TextureType_2D:
        case TextureType_2DMultiSample: {
            textureDesc.DepthOrArraySize = 1;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;            
            break;
        }
        case TextureType_Cube:
            textureDesc.DepthOrArraySize = 6;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            break;
        default:
            std::terminate();
        }

        auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        HRESULT hr = GetDevice()->CreateCommittedResource(
            &heapDesc,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            data.mData ? D3D12_RESOURCE_STATE_COPY_DEST : readStateFlags(),
            isRenderTarget || isDepthTarget ? &clear : nullptr,
            IID_PPV_ARGS(&mTextureHandle.setupAs<ID3D12Resource *>()));
        DX12_CHECK(hr);

        if (data.mData) {
            assert(!isRenderTarget && mType != TextureType_Cube);
            const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mTextureHandle, 0, 1);

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

            DirectX12CommandList list = DirectX12RenderContext::getSingleton().fetchCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

            UpdateSubresources(list, mTextureHandle, uploadHeap, 0, 0, 1, &subResourceDesc);
            list.Transition(mTextureHandle, D3D12_RESOURCE_STATE_COPY_DEST, readStateFlags());

            list.attachResource(std::move(uploadHeap));
            list.execute();
        }

        OffsetPtr handle = DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();
        mResourceBlock.setupAs<D3D12_GPU_DESCRIPTOR_HANDLE>() = DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(handle);
        createShaderResourceView(handle);
    }

    DirectX12Texture::DirectX12Texture(TextureType type, bool isRenderTarget, TextureFormat format, size_t samples)
        : Texture(type, format)
        , mIsRenderTarget(isRenderTarget)
        , mSamples(samples)
    {
    }

    DirectX12Texture::DirectX12Texture(DirectX12Texture &&other)
        : Texture(std::move(other))
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
        std::swap(mIsRenderTarget, other.mIsRenderTarget);
        std::swap(mSamples, other.mSamples);
        return *this;
    }

    void DirectX12Texture::reset()
    {
        if (mTextureHandle) {
            mTextureHandle.release<ID3D12Resource *>()->Release();
            DirectX12RenderContext::getSingleton().mDescriptorHeap.deallocate(DirectX12RenderContext::getSingleton().mDescriptorHeap.fromGpuHandle({ mResourceBlock.release<UINT64>() }));
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
        case FORMAT_RGBA8_SRGB:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            byteCount = 4;
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

        DirectX12CommandList list = DirectX12RenderContext::getSingleton().fetchCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

        list.Transition(mTextureHandle, readStateFlags(), D3D12_RESOURCE_STATE_COPY_DEST);

        CD3DX12_TEXTURE_COPY_LOCATION Dst(mTextureHandle, 0);
        CD3DX12_TEXTURE_COPY_LOCATION Src(uploadHeap, layout);
        list->CopyTextureRegion(&Dst, offset.x, offset.y, 0, &Src, nullptr);

        list.Transition(mTextureHandle, D3D12_RESOURCE_STATE_COPY_DEST, readStateFlags());

        list.attachResource(std::move(uploadHeap));
    }

    void DirectX12Texture::createShaderResourceView(OffsetPtr descriptorHandle) const
    {
        DXGI_FORMAT xFormat;
        switch (mFormat) {
        case FORMAT_RGBA8:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case FORMAT_RGBA16F:
            xFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case FORMAT_RGBA8_SRGB:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case FORMAT_D24:
            xFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        default:
            std::terminate();
        }

        D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc {};
        shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shaderResourceViewDesc.Format = xFormat;

        switch (mType) {
        case TextureType_2D:
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            break;
        case TextureType_2DMultiSample:
            shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
            break;
        case TextureType_Cube:
            shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            shaderResourceViewDesc.TextureCube.MipLevels = 1;
            shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
            shaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
            break;
        default:
            std::terminate();
        }

        GetDevice()->CreateShaderResourceView(mTextureHandle, &shaderResourceViewDesc, DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(descriptorHandle));
        DX12_CHECK();
    }

    DirectX12Texture::operator ID3D12Resource *() const
    {
        return mTextureHandle;
    }

    DirectX12Texture::operator ReleasePtr<ID3D12Resource>() const
    {
        ID3D12Resource *res = mTextureHandle;
        res->AddRef();
        return ReleasePtr<ID3D12Resource> { res };
    }

    void DirectX12Texture::setName(std::string_view name)
    {
        static_cast<ID3D12Resource *>(mTextureHandle)->SetName(StringUtil::toWString(name).c_str());
    }

    D3D12_RESOURCE_STATES DirectX12Texture::readStateFlags() const
    {
        D3D12_RESOURCE_STATES flags = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if (mSamples > 1)
            flags |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        return flags;
    }

    size_t DirectX12Texture::samples() const
    {
        return mSamples;
    }
}
}