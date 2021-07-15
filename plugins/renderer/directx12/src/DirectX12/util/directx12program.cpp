#include "../directx12lib.h"

#include "directx12pixelshader.h"
#include "directx12program.h"
#include "directx12vertexshader.h"

#include "Meta/math/matrix4.h"

#include "../directx12rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX12Program::~DirectX12Program()
    {
        reset();
    }

    DirectX12Program::DirectX12Program(DirectX12Program &&other)
        : mVertexShader(std::move(other.mVertexShader))
        , mPixelShader(std::move(other.mPixelShader))
    {
    }

    DirectX12Program &DirectX12Program::operator=(DirectX12Program &&other)
    {
        std::swap(mVertexShader, other.mVertexShader);
        std::swap(mPixelShader, other.mPixelShader);
        return *this;
    }

    bool DirectX12Program::link(typename DirectX12VertexShaderLoader::HandleType vertexShader, typename DirectX12PixelShaderLoader::HandleType pixelShader)
    {
        reset();

        /*mHandle = glCreateProgram();
        glAttachShader(mHandle, vertexShader->mHandle);
        glAttachShader(mHandle, pixelShader->mHandle);

        for (size_t i = 0; i < attributeNames.size(); ++i) {
            glBindAttribLocation(mHandle, i, attributeNames[i]);
            GL_CHECK();
        }

        glLinkProgram(mHandle);
        // check for linking errors
        GLint success;
        char infoLog[512];
        glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mHandle, 512, NULL, infoLog);
            LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED");
            LOG_ERROR(infoLog);
        }*/

        mVertexShader = std::move(vertexShader);
        mPixelShader = std::move(pixelShader);

        return true;
    }

    void DirectX12Program::reset()
    {
        mVertexShader.reset();
        mPixelShader.reset();
    }

    void DirectX12Program::bind(DirectX12VertexArray *format)
    {
        /*mVertexShader->bind(format);
        mPixelShader->bind();

        for (size_t i = 0; i < mConstantBuffers.size(); ++i) {
            ID3D12Buffer *buffer = mConstantBuffers[i].handle();
            sDeviceContext->VSSetConstantBuffers(i, 1, &buffer);
            sDeviceContext->PSSetConstantBuffers(i, 1, &buffer);
        }
        for (size_t i = 0; i < mDynamicBuffers.size(); ++i) {
            ID3D12Buffer *buffer = mDynamicBuffers[i].handle();
            sDeviceContext->VSSetConstantBuffers(i+3, 1, &buffer);
            sDeviceContext->PSSetConstantBuffers(i+3, 1, &buffer);
        }*/
    }

    void DirectX12Program::setParameters(size_t index, size_t size)
    {
        if (mConstantBuffers.size() <= index)
            mConstantBuffers.resize(index + 1);

        if (!mConstantBuffers[index]) {
            mConstantBuffers[index] = { size };
        } else {
            mConstantBuffers[index].resize(size);
        }
    }

    WritableByteBuffer DirectX12Program::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

	void DirectX12Program::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mDynamicBuffers.size() <= index)
                mDynamicBuffers.resize(index + 1);

        if (!mDynamicBuffers[index]) {
            mDynamicBuffers[index] = {  data.mSize };
        } else {
            mDynamicBuffers[index].resize(data.mSize);
        }

        auto target = mDynamicBuffers[index].mapData();
        std::memcpy(target.mData, data.mData, data.mSize);
    }
}
}