#include "directx12lib.h"

#include "directx12meshloader.h"

#include "directx12meshdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/imageloader/imagedata.h"
#include "Madgine/imageloader/imageloader.h"
#include "Madgine/imageloaderlib.h"
#include "Madgine/meshloader/meshdata.h"

#include "directx12rendercontext.h"

VIRTUALRESOURCELOADERIMPL(Engine::Render::DirectX12MeshLoader, Engine::Render::GPUMeshLoader);

namespace Engine {
namespace Render {

    DirectX12MeshLoader::DirectX12MeshLoader()
    {
        getOrCreateManual("quad", {}, {}, this);
        getOrCreateManual("Cube", {}, {}, this);
        getOrCreateManual("Plane", {}, {}, this);
    }

    Threading::Task<bool> DirectX12MeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        DirectX12MeshData &data = static_cast<DirectX12MeshData &>(_data);

        if (mesh.mVertices.mData)
            data.mVertices.setData(mesh.mVertices, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices, D3D12_RESOURCE_STATE_INDEX_BUFFER);

        co_return co_await GPUMeshLoader::generate(_data, mesh);
    }

    void DirectX12MeshLoader::reset(GPUMeshData &data)
    {
        static_cast<DirectX12MeshData&>(data).mVertices.reset();
        static_cast<DirectX12MeshData &>(data).mIndices.reset();
        GPUMeshLoader::reset(data);
    }

    UniqueResourceBlock DirectX12MeshLoader::createResourceBlock(std::vector<const Texture*> textures)
    {
        return DirectX12RenderContext::getSingleton().createResourceBlock(std::move(textures));
    }

    void DirectX12MeshLoader::destroyResourceBlock(UniqueResourceBlock &block)
    {
        DirectX12RenderContext::getSingleton().destroyResourceBlock(block);
    }

    Threading::TaskQueue *DirectX12MeshLoader::loadingTaskQueue() const
    {
        return DirectX12RenderContext::renderQueue();
    }

}
}
