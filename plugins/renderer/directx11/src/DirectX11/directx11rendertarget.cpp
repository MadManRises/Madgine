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

namespace Engine {
namespace Render {

    DirectX11RenderTarget::DirectX11RenderTarget(DirectX11RenderContext *context)
        : RenderTarget(context)
    {
    }

    DirectX11RenderTarget::~DirectX11RenderTarget()
    {
    }

    void DirectX11RenderTarget::setup(ID3D11RenderTargetView *targetView, const Vector2i &size, const RenderTextureConfig &config)
    {
        if (mTargetView) {
            mTargetView->Release();
            mTargetView = nullptr;
        }

        mTargetView = targetView;

        // Create the depth buffer for use with the depth/stencil view.
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        if (config.mCreateDepthBufferView)
            depthStencilBufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
        depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthStencilBufferDesc.Width = size.x;
        depthStencilBufferDesc.Height = size.y;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        if (mDepthStencilBuffer) {
            mDepthStencilBuffer->Release();
            mDepthStencilBuffer = nullptr;
        }

        HRESULT hr = sDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &mDepthStencilBuffer);
        DX11_CHECK(hr);

        if (mDepthStencilView) {
            mDepthStencilView->Release();
            mDepthStencilView = nullptr;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Flags = 0;
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        hr = sDevice->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDepthStencilView);
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

        if (mDepthStencilBuffer) {
            mDepthStencilBuffer->Release();
            mDepthStencilBuffer = nullptr;
        }

        if (mTargetView) {
            mTargetView->Release();
            mTargetView = nullptr;
        }
    }

    void DirectX11RenderTarget::beginFrame()
    {

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

        sDeviceContext->OMSetRenderTargets(1, &mTargetView, mDepthStencilView);
        sDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
        sDeviceContext->OMSetBlendState(mBlendState, 0, 0xffffffff);

        sDeviceContext->ClearRenderTargetView(mTargetView, color);
        sDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        sDeviceContext->PSSetSamplers(0, 2, mSamplers);

        RenderTarget::beginFrame();
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

    void DirectX11RenderTarget::renderMesh(GPUMeshData *m, Program *p)
    {
        DirectX11MeshData *mesh = static_cast<DirectX11MeshData *>(m);
        DirectX11Program *program = static_cast<DirectX11Program *>(p);

        if (!mesh->mVAO)
            return;

        mesh->mVAO.bind(program);

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
        mesh->mVAO.unbind();
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

    void DirectX11RenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices, TextureHandle texture)
    {
        if (!vertices.empty()) {
            DirectX11MeshData tempMesh;
            DirectX11MeshLoader::getSingleton().generate(tempMesh, { groupSize, std::move(vertices), std::move(indices) });

            tempMesh.mTextureHandle = texture;

            renderMesh(&tempMesh, program);
        }
    }

    void DirectX11RenderTarget::clearDepthBuffer()
    {
        sDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void DirectX11RenderTarget::bindTexture(const std::vector<TextureHandle> &tex)
    {
        sDeviceContext->PSSetShaderResources(0, tex.size(), reinterpret_cast<ID3D11ShaderResourceView *const *>(tex.data()));
    }

}
}
