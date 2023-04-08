#include "vulkanlib.h"

#include "vulkanmeshloader.h"

#include "vulkanmeshdata.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "vulkanrendercontext.h"

UNIQUECOMPONENT(Engine::Render::VulkanMeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::VulkanMeshLoader, Engine::Render::GPUMeshLoader)
METATABLE_END(Engine::Render::VulkanMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::VulkanMeshLoader::Resource, Engine::Render::GPUMeshLoader::Resource)
METATABLE_END(Engine::Render::VulkanMeshLoader::Resource)

namespace Engine {
namespace Render {

    VulkanMeshLoader::VulkanMeshLoader()
    {
        getOrCreateManual("Cube", {}, {}, this);
        getOrCreateManual("Plane", {}, {}, this);
    }

    bool VulkanMeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        VulkanMeshData &data = static_cast<VulkanMeshData &>(_data);

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices);

        return GPUMeshLoader::generate(_data, mesh);
    }

    void VulkanMeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        VulkanMeshData &data = static_cast<VulkanMeshData &>(_data);

        std::memcpy(data.mVertices.mapData(mesh.mVertices.mSize).mData, mesh.mVertices.mData, mesh.mVertices.mSize);

        if (!mesh.mIndices.empty()) {
            std::memcpy(data.mIndices.mapData(mesh.mIndices.size() * sizeof(unsigned short)).mData, mesh.mIndices.data(), mesh.mIndices.size() * sizeof(unsigned short));
        }

        GPUMeshLoader::update(data, mesh);
    }

    void VulkanMeshLoader::reset(GPUMeshData &data)
    {
        static_cast<VulkanMeshData &>(data).reset();
    }

    Threading::TaskQueue *VulkanMeshLoader::loadingTaskQueue() const
    {
        return VulkanRenderContext::renderQueue();
    }

}
}
