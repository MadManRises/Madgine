#pragma once

#include "util/vulkanbuffer.h"
#include "util/vulkantexture.h"

#include "Madgine/meshloader/gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanMeshData : GPUMeshData {


        void reset()
        {
            mMaterials.clear();
            mVertices.reset();
            mIndices.reset();
        }

        VulkanBuffer mVertices;
        VulkanBuffer mIndices;
    };

}
}