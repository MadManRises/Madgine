#pragma once

#include "program.h"

#include "directx11buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11Program : Program {

        DirectX11Program() = default;
        DirectX11Program(DirectX11Program &&other);
        ~DirectX11Program();

        DirectX11Program &operator=(DirectX11Program &&other);

        bool link(DirectX11VertexShader *vertexShader, DirectX11PixelShader *pixelShader);

        void reset();

        void bind(DirectX11VertexArray *format);

        void setParameters(const void *data, size_t size, size_t index);

		void setDynamicParameters(const void *data, size_t size, size_t index);

    private:
        DirectX11VertexShader *mVertexShader = nullptr;
        DirectX11PixelShader *mPixelShader = nullptr;
        std::vector<DirectX11Buffer> mConstantBuffers;
    };

}
}