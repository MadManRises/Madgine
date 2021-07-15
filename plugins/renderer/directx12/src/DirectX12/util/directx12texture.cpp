#include "../directx12lib.h"

#include "directx12texture.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12Texture::DirectX12Texture(TextureType type, DataFormat format, size_t width, size_t height, const ByteBuffer &data)
        : mType(type)
        , mSize { static_cast<int>(width), static_cast<int>(height) }
        , mFormat(format)
    {
        DXGI_FORMAT xFormat;
        D3D12_SRV_DIMENSION dimension;
        size_t byteCount;
        switch (format) {
        case FORMAT_FLOAT8:
            xFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            byteCount = 4;
            break;
        case FORMAT_FLOAT32:
            xFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            byteCount = 16;
            break;
        default:
            std::terminate();
        }

        switch (type) {
        case Texture2D: {
            D3D12_RESOURCE_DESC textureDesc;
            ZeroMemory(&textureDesc, sizeof(D3D12_RESOURCE_DESC));

            textureDesc.DepthOrArraySize = 1;
            textureDesc.Format = xFormat;
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.MipLevels = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            HRESULT hr = sDevice->CreateCommittedResource(
                &heapDesc,
                D3D12_HEAP_FLAG_NONE,
                &textureDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&mResource));
            DX12_CHECK(hr);
            if (data.mData) {
                const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mResource, 0, 1);

                heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
                ID3D12Resource *uploadHeap;
                hr = sDevice->CreateCommittedResource(
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

                UpdateSubresources(DirectX12RenderContext::getSingleton().mTempCommandList.mList, mResource, uploadHeap, 0, 0, 1, &subResourceDesc);
                auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                DirectX12RenderContext::getSingleton().mTempCommandList.mList->ResourceBarrier(1, &transition);

                DirectX12RenderContext::getSingleton().ExecuteCommandList(DirectX12RenderContext::getSingleton().mTempCommandList, [uploadHeap]() { uploadHeap->Release(); });                
            }
            dimension = D3D12_SRV_DIMENSION_TEXTURE2D;

            break;
        }        
        case RenderTarget2D: {
            D3D12_RESOURCE_DESC textureDesc;
            ZeroMemory(&textureDesc, sizeof(D3D12_RESOURCE_DESC));

            textureDesc.DepthOrArraySize = 1;
            textureDesc.Format = xFormat;
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.MipLevels = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            HRESULT hr = sDevice->CreateCommittedResource(
                &heapDesc,
                D3D12_HEAP_FLAG_NONE,
                &textureDesc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                nullptr,
                IID_PPV_ARGS(&mResource));
            DX12_CHECK(hr);
            assert(!data.mData);
            dimension = D3D12_SRV_DIMENSION_TEXTURE2D;

            break;
        }
        default:
            std::terminate();
        }

        D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        ZeroMemory(&shaderResourceViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
        shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shaderResourceViewDesc.Format = xFormat;
        shaderResourceViewDesc.ViewDimension = dimension;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        OffsetPtr handle = DirectX12RenderContext::getSingleton().mDescriptorHeap.allocate();
        mTextureHandle = DirectX12RenderContext::getSingleton().mDescriptorHeap.gpuHandle(handle).ptr;
        sDevice->CreateShaderResourceView(mResource, &shaderResourceViewDesc, DirectX12RenderContext::getSingleton().mDescriptorHeap.cpuHandle(handle));      
    }

    DirectX12Texture::DirectX12Texture(TextureType type, DataFormat format)
        : mType(type)
        , mFormat(format)
    {
    }

    DirectX12Texture::DirectX12Texture(DirectX12Texture &&other)
        : mResource(std::exchange(other.mResource, nullptr))
        , mType(other.mType)
        , mFormat(std::exchange(other.mFormat, {}))
        , mSize(other.mSize)
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
        return *this;
    }

    void DirectX12Texture::reset()
    {
        if (mTextureHandle) {
            mResource->Release();
            mResource = nullptr;
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
        *this = DirectX12Texture { mType, mFormat, static_cast<size_t>(size.x), static_cast<size_t>(size.y), data };
    }

    void DirectX12Texture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        /*size_t byteCount;
        switch (mFormat) {
        case FORMAT_FLOAT8:
            byteCount = 4;
            break;
        case FORMAT_FLOAT32:
            byteCount = 16;
            break;
        default:
            std::terminate();
        }

        auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(data.mSize);
        ID3D12Resource *uploadHeap;
        HRESULT hr = sDevice->CreateCommittedResource(
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

        UpdateSubresources(DirectX12RenderContext::getSingleton().mTempCommandList.mList, mResource, uploadHeap, 0, 0, 1, &subResourceDesc);
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        DirectX12RenderContext::getSingleton().mTempCommandList.mList->ResourceBarrier(1, &transition);

        DirectX12RenderContext::getSingleton().ExecuteCommandList(DirectX12RenderContext::getSingleton().mTempCommandList, [uploadHeap]() { uploadHeap->Release(); }); 
        */

        /*D3D12_BOX box;
        box.top = offset.y;
        box.left = offset.x;
        box.right = offset.x + size.x;
        box.bottom = offset.y + size.y;
        box.front = 0;
        box.back = 1;

        HRESULT hr = mResource->Map(0, nullptr, nullptr);
        DX12_CHECK(hr);
        hr = mResource->WriteToSubresource(0, &box, data.mData, size.x * byteCount, 0);
        DX12_CHECK(hr);
        mResource->Unmap(0, nullptr);*/
        LOG_ONCE("Fix setSubData");
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
}
}