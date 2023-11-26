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
        static_cast<VulkanMeshData &>(data).reset();
    }

    UniqueResourceBlock VulkanMeshLoader::createResourceBlock(std::vector<const Texture*> textures)
    {
        return VulkanRenderContext::getSingleton().createResourceBlock(std::move(textures));
    }

    Threading::TaskQueue *VulkanMeshLoader::loadingTaskQueue() const
    {
        return VulkanRenderContext::renderQueue();
    }

}
}
