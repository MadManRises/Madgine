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

METATABLE_BEGIN_BASE(Engine::Render::DirectX12MeshLoader::Resource, Engine::Render::GPUMeshLoader::Resource)
METATABLE_END(Engine::Render::DirectX12MeshLoader::Resource)

namespace Engine {
namespace Render {

    DirectX12MeshLoader::DirectX12MeshLoader()
    {
        getOrCreateManual("Cube", {}, {}, this);
        getOrCreateManual("Plane", {}, {}, this);
    }

    bool DirectX12MeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices);

        return GPUMeshLoader::generate(_data, mesh);
    }

    void DirectX12MeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);

        std::memcpy(data.mVertices.mapData(mesh.mVertices.mSize).mData, mesh.mVertices.mData, mesh.mVertices.mSize);

        if (!mesh.mIndices.empty()) {
            std::memcpy(data.mIndices.mapData(mesh.mIndices.size() * sizeof(unsigned short)).mData, mesh.mIndices.data(), mesh.mIndices.size() * sizeof(unsigned short));
        }

        GPUMeshLoader::update(data, mesh);
    }

    void DirectX12MeshLoader::reset(GPUMeshData &data)
    {
        static_cast<DirectX12MeshData &>(data).reset();
    }

}
}
