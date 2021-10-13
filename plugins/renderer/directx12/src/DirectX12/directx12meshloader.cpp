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
        data.mVertices.setData(mesh.mVertices);

        data.mVertices.bindVertex(data.mVAO.mStride);

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mIndices.setData(mesh.mIndices);

            data.mIndices.bindIndex();
            data.mElementCount = mesh.mIndices.size();
        }

        generateMaterials(data, mesh);

        data.mVAO = { mesh.mGroupSize, data.mVertices, data.mIndices, mesh.mAttributeList() };
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
        data.mVAO.mGroupSize = mesh.mGroupSize;

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
