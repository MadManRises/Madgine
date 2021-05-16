#include "directx11lib.h"

#include "directx11meshloader.h"

#include "directx11meshdata.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imageloaderlib.h"
#include "imagedata.h"
#include "imageloader.h"
#include "meshdata.h"

#include "directx11rendercontext.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11MeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX11MeshLoader, Engine::Render::GPUMeshLoader)
METATABLE_END(Engine::Render::DirectX11MeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11MeshLoader::ResourceType, Engine::Render::GPUMeshLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX11MeshLoader::ResourceType)

namespace Engine {
namespace Render {

    DirectX11MeshLoader::DirectX11MeshLoader()
    {
    }

    void Engine::Render::DirectX11MeshLoader::generateImpl(DirectX11MeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVAO = { mesh.mAttributeList() };
        data.mVAO.bind();

        data.mVertices = {
            D3D11_BIND_VERTEX_BUFFER,
            mesh.mVertices
        };
        data.mVertices.bindVertex(data.mVAO.mStride);

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mIndices = {
                D3D11_BIND_INDEX_BUFFER,
                mesh.mIndices
            };
            data.mIndices.bindIndex();
            data.mElementCount = mesh.mIndices.size();
        }

        if (!mesh.mTexturePath.empty()) {
            std::string_view imageName = mesh.mTexturePath.stem();
            Resources::ImageLoader::HandleType tex;
            tex.load(imageName);

            data.mTexture = { Texture2D, FORMAT_FLOAT8, D3D11_BIND_SHADER_RESOURCE, static_cast<size_t>(tex->mWidth), static_cast<size_t>(tex->mHeight), { tex->mBuffer, static_cast<size_t>(tex->mWidth * tex->mHeight) } };
            data.mTextureHandle = data.mTexture.handle();
        }

        data.mVAO.unbind();
    }

    bool DirectX11MeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);
        data.mAABB = mesh.mAABB;

        DirectX11RenderContext::execute([=, &data, &mesh]() mutable {
            generateImpl(data, mesh);
        });

        return true;
    }

    bool DirectX11MeshLoader::generate(GPUMeshData &_data, MeshData &&mesh)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);
        data.mAABB = mesh.mAABB;

        DirectX11RenderContext::execute([=, &data, mesh { std::move(mesh) }]() mutable {
            generateImpl(data, mesh);
        });

        return true;
    }

    void DirectX11MeshLoader::updateImpl(DirectX11MeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVertices = {
            D3D11_BIND_VERTEX_BUFFER,
            mesh.mVertices
        };

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mIndices = {
                D3D11_BIND_INDEX_BUFFER,
                mesh.mIndices
            };
            data.mElementCount = mesh.mIndices.size();
        }
    }

    void DirectX11MeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);

        data.mAABB = mesh.mAABB;

        DirectX11RenderContext::execute([=, &data, &mesh]() mutable {
            updateImpl(data, mesh);
        });
    }

    void DirectX11MeshLoader::update(GPUMeshData &_data, MeshData &&mesh)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);

        data.mAABB = mesh.mAABB;

        DirectX11RenderContext::execute([=, &data, mesh { std::move(mesh) }]() mutable {
            updateImpl(data, mesh);
        });
    }

    void DirectX11MeshLoader::reset(GPUMeshData &data)
    {
        static_cast<DirectX11MeshData &>(data).reset();
    }

}
}
