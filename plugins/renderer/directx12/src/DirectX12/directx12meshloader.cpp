#include "directx12lib.h"

#include "directx12meshloader.h"

#include "directx12meshdata.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imageloaderlib.h"
#include "imagedata.h"
#include "imageloader.h"
#include "meshdata.h"

#include "directx12rendercontext.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX12MeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX12MeshLoader, Engine::Render::GPUMeshLoader)
METATABLE_END(Engine::Render::DirectX12MeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12MeshLoader::ResourceType, Engine::Render::GPUMeshLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX12MeshLoader::ResourceType)

namespace Engine {
namespace Render {

    DirectX12MeshLoader::DirectX12MeshLoader()
    {
    }

    void Engine::Render::DirectX12MeshLoader::generateImpl(DirectX12MeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVAO = { mesh.mAttributeList() };
        data.mVAO.bind();

        DX12_CHECK();

        bool dynamic = !mesh.mVertices.mData;

        if (!dynamic) {
            data.mVertices = {
                mesh.mVertices
            };
        } else {
            data.mVertices = {
                mesh.mVertices.mSize
            };
        }
        data.mVertices.bindVertex(data.mVAO.mStride);

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            if (!dynamic) {
                data.mIndices = {
                    mesh.mIndices
                };
            } else {
                data.mIndices = {
                    mesh.mIndices.size() * sizeof(unsigned short)
                };
            }
            data.mIndices.bindIndex();
            data.mElementCount = mesh.mIndices.size();
        }

        if (!mesh.mTexturePath.empty()) {
            std::string_view imageName = mesh.mTexturePath.stem();
            Resources::ImageLoader::HandleType tex;
            tex.load(imageName);

            data.mTexture = { Texture2D, FORMAT_FLOAT8, static_cast<size_t>(tex->mWidth), static_cast<size_t>(tex->mHeight), { tex->mBuffer, static_cast<size_t>(tex->mWidth * tex->mHeight) } };
            data.mTextureHandle = data.mTexture.mTextureHandle;
        }

        data.mVAO.unbind();
    }

    bool DirectX12MeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);
        data.mAABB = mesh.mAABB;

        DirectX12RenderContext::execute([=, &data, &mesh]() mutable {
            generateImpl(data, mesh);
        });

        return true;
    }

    bool DirectX12MeshLoader::generate(GPUMeshData &_data, MeshData &&mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);
        data.mAABB = mesh.mAABB;

        DirectX12RenderContext::execute([=, &data, mesh { std::move(mesh) }]() mutable {
            generateImpl(data, mesh);
        });

        return true;
    }

    void DirectX12MeshLoader::updateImpl(DirectX12MeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVertices.resize(mesh.mVertices.mSize);
        std::memcpy(data.mVertices.mapData().mData, mesh.mVertices.mData, mesh.mVertices.mSize);

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mIndices.resize(mesh.mIndices.size() * sizeof(unsigned short));
            std::memcpy(data.mIndices.mapData().mData, mesh.mIndices.data(), mesh.mIndices.size() * sizeof(unsigned short));
            data.mElementCount = mesh.mIndices.size();
        }
    }

    void DirectX12MeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);

        data.mAABB = mesh.mAABB;

        DirectX12RenderContext::execute([=, &data, &mesh]() mutable {
            updateImpl(data, mesh);
        });
    }

    void DirectX12MeshLoader::update(GPUMeshData &_data, MeshData &&mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);

        data.mAABB = mesh.mAABB;

        DirectX12RenderContext::execute([=, &data, mesh { std::move(mesh) }]() mutable {
            updateImpl(data, mesh);
        });
    }

    void DirectX12MeshLoader::reset(GPUMeshData &data)
    {
        static_cast<DirectX12MeshData &>(data).reset();
    }

}
}
