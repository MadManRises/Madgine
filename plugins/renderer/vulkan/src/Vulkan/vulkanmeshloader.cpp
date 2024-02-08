#include "vulkanlib.h"

#include "vulkanmeshloader.h"

#include "vulkanmeshdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "vulkanrendercontext.h"

VIRTUALRESOURCELOADERIMPL(Engine::Render::VulkanMeshLoader, Engine::Render::GPUMeshLoader);

namespace Engine {
namespace Render {

    VulkanMeshLoader::VulkanMeshLoader()
    {
        getOrCreateManual("quad", {}, {}, this);
        getOrCreateManual("Cube", {}, {}, this);
        getOrCreateManual("Plane", {}, {}, this);
    }

    Threading::Task<bool> VulkanMeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        VulkanMeshData &data = static_cast<VulkanMeshData &>(_data);

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices);

        co_return co_await GPUMeshLoader::generate(_data, mesh);
    }

    void VulkanMeshLoader::reset(GPUMeshData &data)
    {
        static_cast<VulkanMeshData &>(data).mVertices.reset();
        static_cast<VulkanMeshData &>(data).mIndices.reset();
        GPUMeshLoader::reset(data);
    }

    UniqueResourceBlock VulkanMeshLoader::createResourceBlock(std::vector<const Texture*> textures)
    {
        return VulkanRenderContext::getSingleton().createResourceBlock(std::move(textures));
    }

    void VulkanMeshLoader::destroyResourceBlock(UniqueResourceBlock &block)
    {
        VulkanRenderContext::getSingleton().destroyResourceBlock(block);
    }

    Threading::TaskQueue *VulkanMeshLoader::loadingTaskQueue() const
    {
        return VulkanRenderContext::renderQueue();
    }

}
}
