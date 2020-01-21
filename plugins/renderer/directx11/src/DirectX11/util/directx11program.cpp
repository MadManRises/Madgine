#include "../directx11lib.h"

#include "directx11pixelshader.h"
#include "directx11program.h"
#include "directx11vertexshader.h"

#include "Modules/math/matrix4.h"

#include "../directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11Program::~DirectX11Program()
    {
        reset();
    }

    DirectX11Program::DirectX11Program(DirectX11Program &&other)
        : mVertexShader(std::exchange(other.mVertexShader, nullptr))
        , mPixelShader(std::exchange(other.mPixelShader, nullptr))
    {
    }

    DirectX11Program &DirectX11Program::operator=(DirectX11Program &&other)
    {
        std::swap(mVertexShader, other.mVertexShader);
        std::swap(mPixelShader, other.mPixelShader);
        return *this;
    }

    bool DirectX11Program::link(DirectX11VertexShader *vertexShader, DirectX11PixelShader *pixelShader)
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

        mVertexShader = vertexShader;
        mPixelShader = pixelShader;

        return true;
    }

    void DirectX11Program::reset()
    {
        mVertexShader = nullptr;
        mPixelShader = nullptr;
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
    }

    void DirectX11Program::setParameters(const void *data, size_t size, size_t index)
    {
        if (mConstantBuffers.size() <= index)
            mConstantBuffers.resize(index + 1);

        if (!mConstantBuffers[index]) {
            mConstantBuffers[index] = { D3D11_BIND_CONSTANT_BUFFER, size, data };
        } else {
            mConstantBuffers[index].setData(size, data);
        }
    }
}
}