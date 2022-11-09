#include "../directx12lib.h"

#include "directx12texture.h"

#include "../directx12rendercontext.h"

#include "Generic/align.h"

namespace Engine {
namespace Render {

    DirectX12Texture::DirectX12Texture(TextureType type, bool isRenderTarget, DataFormat format, size_t width, size_t height, const ByteBuffer &data)
        : mType(type)
        , mSize { static_cast<int>(width), static_cast<int>(height) }
        , mFormat(format)
        , mIsRenderTarget(isRenderTarget)
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

        switch (type) {
        case TextureType_2D: {
            D3D12_RESOURCE_DESC textureDesc {};

            textureDesc.DepthOrArraySize = 1;
            textureDesc.Format = xFormat;
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.MipLevels = 1;
            textureDesc.SampleDesc.Count = 1;
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

                DirectX12CommandList &list = DirectX12RenderContext::getSingleton().mTempCommandList;

                UpdateSubresources(list.mList, mResource, uploadHeap, 0, 0, 1, &subResourceDesc);
                auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                list.mList->ResourceBarrier(1, &transition);

                DirectX12RenderContext::getSingleton().ExecuteCommandList(list, [uploadHeap { std::move(uploadHeap) }]() {});
            }
            dimension = D3D12_SRV_DIMENSION_TEXTURE2D;

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

    DirectX12Texture::DirectX12Texture(TextureType type, bool isRenderTarget, DataFormat format)
        : mType(type)
        , mFormat(format)
        , mIsRenderTarget(isRenderTarget)
    {
    }

    DirectX12Texture::DirectX12Texture(DirectX12Texture &&other)
        : mResource(std::exchange(other.mResource, nullptr))
        , mType(other.mType)
        , mFormat(std::exchange(other.mFormat, {}))
        , mSize(other.mSize)
        , mIsRenderTarget(std::exchange(other.mIsRenderTarget, false))
    {
        mTextureHandle = std::exchange(other.mTextureHandle, 0);
    }

    DirectX12Texture::~DirectX12Texture()
    {
        reset();
    }

    DirectX12Texture &DirectX12Texture::operator=(DirectX12Texture &&other)
    {
        std::swap(mResource, other.mResource);
        std::swap(mTextureHandle, other.mTextureHandle);
        std::swap(mType, other.mType);
        std::swap(mFormat, other.mFormat);
        std::swap(mSize, other.mSize);
        std::swap(mIsRenderTarget, other.mIsRenderTarget);
        return *this;
    }

    void DirectX12Texture::reset()
    {
        if (mTextureHandle) {
            mResource.reset();
            DirectX12RenderContext::getSingleton().mDescriptorHeap.deallocate(DirectX12RenderContext::getSingleton().mDescriptorHeap.fromGpuHandle({ mTextureHandle }));
            mTextureHandle = 0;
        }
    }

    void DirectX12Texture::bind() const
    {
        throw 0;
        //sDeviceContext->PSSetShaderResources(0, 1, reinterpret_cast<ID3D12ShaderResourceView *const *>(&mTextureHandle));
    }

    void DirectX12Texture::setData(Vector2i size, const ByteBuffer &data)
    {
        *this = DirectX12Texture { mType, mIsRenderTarget, mFormat, static_cast<size_t>(size.x), static_cast<size_t>(size.y), data };
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

        DirectX12CommandList &list = DirectX12RenderContext::getSingleton().mTempCommandList;

        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
        list.mList->ResourceBarrier(1, &transition);

        CD3DX12_TEXTURE_COPY_LOCATION Dst(mResource, 0);
        CD3DX12_TEXTURE_COPY_LOCATION Src(uploadHeap, layout);
        list.mList->CopyTextureRegion(&Dst, offset.x, offset.y, 0, &Src, nullptr);

        transition = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        list.mList->ResourceBarrier(1, &transition);

        DirectX12RenderContext::getSingleton().ExecuteCommandList(list, [uploadHeap { std::move(uploadHeap) }]() {});
    }

    void DirectX12Texture::resize(Vector2i size)
    {
        throw "TODO";
        /*Vector2i commonSize = min(size, mSize);
        GLuint tempTex;
        glGenTextures(1, &tempTex);
        GL_CHECK();
        std::swap(tempTex, mHandle);

        setData(size, nullptr);

        glCopyImageSubData(tempTex, GL_TEXTURE_2D, 0, 0, 0, 0, mHandle, GL_TEXTURE_2D, 0, 0, 0, 0, commonSize.x, commonSize.y, 1);
        GL_CHECK();

        glDeleteTextures(1, &tempTex);
        GL_CHECK();*/
    }

    ID3D12Resource *DirectX12Texture::resource() const
    {
        return mResource;
    }

    TextureDescriptor DirectX12Texture::descriptor() const
    {
        return { mTextureHandle, mType };
    }

    /*void DirectX12Texture::setWrapMode(GLint mode)
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
        GL_CHECK();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        GL_CHECK();
    }

    void DirectX12Texture::setFilter(GLint filter)
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        GL_CHECK();
    }*/

    void DirectX12Texture::setName(std::string_view name)
    {
        mResource->SetName(StringUtil::toWString(name).c_str());
    }
}
}