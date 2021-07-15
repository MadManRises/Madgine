#pragma once

#include "program.h"

#include "../directx11pixelshaderloader.h"
#include "../directx11vertexshaderloader.h"
#include "directx11buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11Program : Program {

        DirectX11Program() = default;
        DirectX11Program(DirectX11Program &&other);
        ~DirectX11Program();

        DirectX11Program &operator=(DirectX11Program &&other);

        bool link(typename DirectX11VertexShaderLoader::HandleType vertexShader, typename DirectX11PixelShaderLoader::HandleType pixelShader);

        void reset();

        void bind(DirectX11VertexArray *format);

        void setParameters(size_t index, size_t size);

        WritableByteBuffer mapParameters(size_t index);

        void setDynamicParameters(size_t index, const ByteBuffer &data);

    private:
        DirectX11VertexShaderLoader::HandleType mVertexShader;
        DirectX11PixelShaderLoader::HandleType mPixelShader;
        std::vector<DirectX11Buffer> mConstantBuffers;
        std::vector<DirectX11Buffer> mDynamicBuffers;
    };

}
}