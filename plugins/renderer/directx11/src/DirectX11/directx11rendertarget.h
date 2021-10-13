#pragma once

#include "Madgine/render/rendertarget.h"
#include "Meta/math/matrix4.h"
#include "util/directx11buffer.h"
#include "util/directx11program.h"
#include "util/directx11texture.h"

#include "render/renderpassflags.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTarget : RenderTarget {

        DirectX11RenderTarget(DirectX11RenderContext *context, bool global, std::string name, size_t iterations = 1);
        ~DirectX11RenderTarget();

        void setup(std::vector<ID3D11RenderTargetView *> targetViews, const Vector2i &size, TextureType type, size_t samples = 1);
        void shutdown();

        virtual void beginIteration(size_t iteration) override;
        virtual void endIteration(size_t iteration) override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, const GPUMeshData::Material *material = nullptr) override;
        virtual void renderMesh(GPUMeshData *mesh, Program *program, const GPUMeshData::Material *material = nullptr) override;
        virtual void renderMeshInstanced(size_t count, GPUMeshData *mesh, Program *program, const GPUMeshData::Material *material = nullptr) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) override;        

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        std::vector<ID3D11RenderTargetView *> mTargetViews;
        DirectX11Texture mDepthBuffer;       
        ID3D11DepthStencilView *mDepthStencilView = nullptr;
        ID3D11DepthStencilState *mDepthStencilState = nullptr;
        ID3D11RasterizerState *mRasterizerState = nullptr;
        ID3D11BlendState *mBlendState = nullptr;

        ID3D11SamplerState *mSamplers[2] = { nullptr, nullptr };
        
    };

}
}