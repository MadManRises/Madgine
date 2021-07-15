#include "../directx11lib.h"

#include "directx11pixelshader.h"
#include "directx11program.h"
#include "directx11vertexshader.h"

#include "Meta/math/matrix4.h"

#include "../directx11rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX11Program::~DirectX11Program()
    {
        reset();
    }

    DirectX11Program::DirectX11Program(DirectX11Program &&other)
        : mVertexShader(std::move(other.mVertexShader))
        , mPixelShader(std::move(other.mPixelShader))
    {
    }

    DirectX11Program &DirectX11Program::operator=(DirectX11Program &&other)
    {
        std::swap(mVertexShader, other.mVertexShader);
        std::swap(mPixelShader, other.mPixelShader);
        return *this;
    }

    bool DirectX11Program::link(typename DirectX11VertexShaderLoader::HandleType vertexShader, typename DirectX11PixelShaderLoader::HandleType pixelShader)
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

    void DirectX11Program::reset()
    {
        mVertexShader.reset();
        mPixelShader.reset();
    }

    void DirectX11Program::bind(DirectX11VertexArray *format)
    {
        mVertexShader->bind(format);
        mPixelShader->bind();

        for (size_t i = 0; i < mConstantBuffers.size(); ++i) {
            ID3D11Buffer *buffer = mConstantBuffers[i].handle();
            sDeviceContext->VSSetConstantBuffers(i, 1, &buffer);
            sDeviceContext->PSSetConstantBuffers(i, 1, &buffer);
        }
        for (size_t i = 0; i < mDynamicBuffers.size(); ++i) {
            ID3D11Buffer *buffer = mDynamicBuffers[i].handle();
            sDeviceContext->VSSetConstantBuffers(i+3, 1, &buffer);
            sDeviceContext->PSSetConstantBuffers(i+3, 1, &buffer);
        }
    }

    void DirectX11Program::setParameters(size_t index, size_t size)
    {
        if (mConstantBuffers.size() <= index)
            mConstantBuffers.resize(index + 1);

        if (!mConstantBuffers[index]) {
            mConstantBuffers[index] = { D3D11_BIND_CONSTANT_BUFFER, size };
        } else {
            mConstantBuffers[index].resize(size);
        }
    }

    WritableByteBuffer DirectX11Program::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

	void DirectX11Program::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mDynamicBuffers.size() <= index)
                mDynamicBuffers.resize(index + 1);

        if (!mDynamicBuffers[index]) {
            mDynamicBuffers[index] = { D3D11_BIND_CONSTANT_BUFFER, data.mSize };
        } else {
            mDynamicBuffers[index].resize(data.mSize);
        }

        if (data.mSize > 0) {
            auto target = mDynamicBuffers[index].mapData();
            std::memcpy(target.mData, data.mData, data.mSize);
        }
    }
}
}