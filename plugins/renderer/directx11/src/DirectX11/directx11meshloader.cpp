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

METATABLE_BEGIN_BASE(Engine::Render::DirectX11MeshLoader::Resource, Engine::Render::GPUMeshLoader::Resource)
METATABLE_END(Engine::Render::DirectX11MeshLoader::Resource)

namespace Engine {
namespace Render {

    DirectX11MeshLoader::DirectX11MeshLoader()
    {
        getOrCreateManual("Plane", {}, {}, this);
    }

    bool Engine::Render::DirectX11MeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices);

        return GPUMeshLoader::generate(data, mesh);
    }

    void DirectX11MeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);

        data.mVertices.resize(mesh.mVertices.mSize);
        std::memcpy(data.mVertices.mapData().mData, mesh.mVertices.mData, mesh.mVertices.mSize);

        if (!mesh.mIndices.empty()) {
            data.mIndices.resize(mesh.mIndices.size() * sizeof(unsigned short));
            std::memcpy(data.mIndices.mapData().mData, mesh.mIndices.data(), mesh.mIndices.size() * sizeof(unsigned short));
        }

        GPUMeshLoader::update(data, mesh);
    }

    void DirectX11MeshLoader::reset(GPUMeshData &data)
    {
        static_cast<DirectX11MeshData &>(data).reset();
    }

    Threading::TaskQueue *DirectX11MeshLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}
