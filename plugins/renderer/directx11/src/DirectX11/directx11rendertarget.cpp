#include "directx11lib.h"

#include "directx11rendertarget.h"

#include "Meta/math/matrix4.h"
#include "Meta/math/vector4.h"

#include "meshloader.h"

#include "directx11renderwindow.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/renderpass.h"
#include "directx11meshdata.h"
#include "directx11meshloader.h"

#include "directx11rendercontext.h"

#include "render/material.h"

namespace Engine {
namespace Render {

    DirectX11RenderTarget::DirectX11RenderTarget(DirectX11RenderContext *context, bool global, std::string name, size_t iterations)
        : RenderTarget(context, global, name, iterations)
    {
    }

    DirectX11RenderTarget::~DirectX11RenderTarget()
    {
    }

    void DirectX11RenderTarget::setup(std::vector<ID3D11RenderTargetView *> targetViews, const Vector2i &size, TextureType type, size_t samples)
    {
        for (ID3D11RenderTargetView *view : mTargetViews) {
            view->Release();
        }
        mTargetViews = std::move(targetViews);        

        mDepthBuffer = { type, FORMAT_D32, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, static_cast<size_t>(size.x), static_cast<size_t>(size.y), samples };

        if (mDepthStencilView) {
            mDepthStencilView->Release();
            mDepthStencilView = nullptr;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Flags = 0;
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        switch (type) {
        case TextureType_2D:
            dsvDesc.ViewDimension = samples ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
            LOG_ONCE("Fix this!");
            break;
        case TextureType_2DMultiSample:
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            break;
        case TextureType_Cube:
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvDesc.Texture2DArray.ArraySize = 6;
            dsvDesc.Texture2DArray.FirstArraySlice = 0;
            break;
        default:
            throw 0;
        }

        HRESULT hr = sDevice->CreateDepthStencilView(mDepthBuffer.resource(), &dsvDesc, &mDepthStencilView);
        DX11_CHECK(hr);

        if (!mDepthStencilState) {

            // Setup depth/stencil state.
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
            ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

            depthStencilStateDesc.DepthEnable = TRUE;
            depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilStateDesc.StencilEnable = FALSE;

            hr = sDevice->CreateDepthStencilState(&depthStencilStateDesc, &mDepthStencilState);
            DX11_CHECK(hr);
        }

        if (!mRasterizerState) {

            // Setup rasterizer state.
            D3D11_RASTERIZER_DESC rasterizerDesc;
            ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

            rasterizerDesc.AntialiasedLineEnable = FALSE;
            rasterizerDesc.CullMode = /*D3D11_CULL_BACK*/ D3D11_CULL_NONE;
            rasterizerDesc.DepthBias = 0;
            rasterizerDesc.DepthBiasClamp = 0.0f;
            rasterizerDesc.DepthClipEnable = FALSE;
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.FrontCounterClockwise = FALSE;
            rasterizerDesc.MultisampleEnable = FALSE;
            rasterizerDesc.ScissorEnable = FALSE;
            rasterizerDesc.SlopeScaledDepthBias = 0.0f;

            // Create the rasterizer state object.
            hr = sDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState);
            DX11_CHECK(hr);
        }

        if (!mBlendState) {
            D3D11_BLEND_DESC omDesc;
            ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));

            omDesc.RenderTarget[0].BlendEnable = true;
            omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            hr = sDevice->CreateBlendState(&omDesc, &mBlendState);
            DX11_CHECK(hr);
        }

        if (!mSamplers[0]) {

            D3D11_SAMPLER_DESC samplerDesc;

            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.MaxAnisotropy = 1;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            samplerDesc.BorderColor[0] = 0;
            samplerDesc.BorderColor[1] = 0;
            samplerDesc.BorderColor[2] = 0;
            samplerDesc.BorderColor[3] = 0;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

            hr = sDevice->CreateSamplerState(&samplerDesc, mSamplers + 0);
            DX11_CHECK(hr);

            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

            hr = sDevice->CreateSamplerState(&samplerDesc, mSamplers + 1);
            DX11_CHECK(hr);
        }
    }

    void DirectX11RenderTarget::shutdown()
    {
        for (size_t i = 0; i < 2; ++i) {
            if (mSamplers[i]) {
                mSamplers[i]->Release();
                mSamplers[i] = nullptr;
            }
        }

        if (mBlendState) {
            mBlendState->Release();
            mBlendState = nullptr;
        }

        if (mRasterizerState) {
            mRasterizerState->Release();
            mRasterizerState = nullptr;
        }

        if (mDepthStencilState) {
            mDepthStencilState->Release();
            mDepthStencilState = nullptr;
        }

        if (mDepthStencilView) {
            mDepthStencilView->Release();
            mDepthStencilView = nullptr;
        }

        for (ID3D11RenderTargetView *view : mTargetViews) {
            view->Release();
        }
        mTargetViews.clear();
    }

    void DirectX11RenderTarget::beginIteration(size_t iteration)
    {
        RenderTarget::beginIteration(iteration);

        const Vector2i &screenSize = size();

        sDeviceContext->RSSetState(mRasterizerState);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(screenSize.x);
        viewport.Height = static_cast<float>(screenSize.y);
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        sDeviceContext->RSSetViewports(1, &viewport);

        constexpr FLOAT color[4] = { 0.2f, 0.3f, 0.3f, 1.0f };

        int bufferCount = iterations() > 1 ? 2 : 1;
        int offset = iterations() > 1 ? iteration % 2 : 0;
        size_t size = mTargetViews.size() / bufferCount;
        sDeviceContext->OMSetRenderTargets(size, mTargetViews.data() + size * offset, mDepthStencilView);
        sDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
        sDeviceContext->OMSetBlendState(mBlendState, 0, 0xffffffff);

        for (ID3D11RenderTargetView *view : std::span { mTargetViews.begin() + size * offset, size })
            sDeviceContext->ClearRenderTargetView(view, color);
        sDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        sDeviceContext->PSSetSamplers(0, 2, mSamplers);

        LOG_DEBUG("Begin Iteration");
    }

    void DirectX11RenderTarget::endIteration(size_t iteration)
    {
        LOG_DEBUG("End Iteration");
        sDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
        ID3D11ShaderResourceView *nullSRV[5] = { nullptr, nullptr, nullptr, nullptr };
        sDeviceContext->PSSetShaderResources(0, 5, nullSRV);

        RenderTarget::endIteration(iteration);
    }

    void DirectX11RenderTarget::pushAnnotation(const char *tag)
    {
        sAnnotator->BeginEvent(StringUtil::toWString(tag).c_str());
    }

    void DirectX11RenderTarget::popAnnotation()
    {
        sAnnotator->EndEvent();
    }

    void DirectX11RenderTarget::setRenderSpace(const Rect2i &space)
    {
        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(space.mSize.x);
        viewport.Height = static_cast<float>(space.mSize.y);
        viewport.TopLeftX = static_cast<float>(space.mTopLeft.x);
        viewport.TopLeftY = static_cast<float>(space.mTopLeft.y);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        sDeviceContext->RSSetViewports(1, &viewport);
    }

    void DirectX11RenderTarget::renderMesh(GPUMeshData *m, Program *p, const Material *material)
    {
        DirectX11MeshData *mesh = static_cast<DirectX11MeshData *>(m);
        DirectX11Program *program = static_cast<DirectX11Program *>(p);

        if (!mesh->mVAO)
            return;

        program->bind(&mesh->mVAO);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D }, { material->mEmissiveTexture, TextureType_2D } });

        constexpr D3D11_PRIMITIVE_TOPOLOGY modes[] {
            D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mesh->mGroupSize > 0 && mesh->mGroupSize <= 3);
        D3D11_PRIMITIVE_TOPOLOGY mode = modes[mesh->mGroupSize - 1];
        sDeviceContext->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            sDeviceContext->DrawIndexed(mesh->mElementCount, 0, 0);
        } else {
            sDeviceContext->Draw(mesh->mElementCount, 0);
        }      
    }

    void DirectX11RenderTarget::renderMeshInstanced(size_t count, GPUMeshData *m, Program *p, const Material *material)
    {
        DirectX11MeshData *mesh = static_cast<DirectX11MeshData *>(m);
        DirectX11Program *program = static_cast<DirectX11Program *>(p);

        if (!mesh->mVAO)
            return;

        mesh->mVAO.bind();
        program->bind(&mesh->mVAO);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D }, { material->mEmissiveTexture, TextureType_2D } });

        constexpr D3D11_PRIMITIVE_TOPOLOGY modes[] {
            D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mesh->mGroupSize > 0 && mesh->mGroupSize <= 3);
        D3D11_PRIMITIVE_TOPOLOGY mode = modes[mesh->mGroupSize - 1];
        sDeviceContext->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            sDeviceContext->DrawIndexedInstanced(mesh->mElementCount, count, 0, 0, 0);
        } else {
            sDeviceContext->DrawInstanced(mesh->mElementCount, count, 0, 0);
        }
    }

    void DirectX11RenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices)
    {
        if (!vertices.empty()) {
            DirectX11MeshData tempMesh;
            DirectX11MeshLoader::getSingleton().generate(tempMesh, { groupSize, std::move(vertices), std::move(indices) });

            //setupProgram(flags);

            renderMesh(&tempMesh, program);
        }
    }

    void DirectX11RenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices, const Material *material)
    {
        if (!vertices.empty()) {
            DirectX11MeshData tempMesh;
            DirectX11MeshLoader::getSingleton().generate(tempMesh, { groupSize, std::move(vertices), std::move(indices) });

            renderMesh(&tempMesh, program, material);
        }
    }

    void DirectX11RenderTarget::clearDepthBuffer()
    {
        sDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void DirectX11RenderTarget::bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset)
    {
        LOG_DEBUG("Texture Bind");
        std::vector<ID3D11ShaderResourceView *> handles;
        std::transform(tex.begin(), tex.end(), std::back_inserter(handles), [](const TextureDescriptor &desc) {
            return reinterpret_cast<ID3D11ShaderResourceView*>(desc.mTextureHandle);
        });
        sDeviceContext->PSSetShaderResources(offset, tex.size(), handles.data());
    }

}
}
