#pragma once

#include "Madgine/render/rendertarget.h"
#include "Modules/math/matrix4.h"
#include "util/directx11buffer.h"
#include "util/directx11program.h"
#include "util/directx11texture.h"

#include "Modules/render/renderpassflags.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTarget : RenderTarget {

        DirectX11RenderTarget(DirectX11RenderContext *context);
        ~DirectX11RenderTarget();

        void setup(ID3D11RenderTargetView *targetView, const Vector2i &size);
        void shutdown();

        virtual void beginFrame() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(Program *program, RenderPassFlags flags, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) override;
        virtual void renderVertices(Program *program, RenderPassFlags flags, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, TextureHandle texture = 0) override;
        virtual void renderMesh(MeshData *mesh, Program *program) override;
        virtual void clearDepthBuffer() override;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        ID3D11RenderTargetView *mTargetView = nullptr;
        ID3D11Texture2D *mDepthStencilBuffer = nullptr;
        ID3D11DepthStencilView *mDepthStencilView = nullptr;
        ID3D11DepthStencilState *mDepthStencilState = nullptr;
        ID3D11RasterizerState *mRasterizerState = nullptr;
        ID3D11BlendState *mBlendState = nullptr;
        
    };

}
}