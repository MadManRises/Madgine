#pragma once

#include "Madgine_Tools/toolbase.h"
#include "Madgine_Tools/toolscollector.h"

#include "Vulkan/util/vulkantexture.h"
#include "Madgine_Tools/render/rendercontexttool.h"

namespace Engine {
namespace Tools {

    struct VulkanRenderContextTool : public ToolVirtualImpl<VulkanRenderContextTool, RenderContextTool> {

        SERIALIZABLEUNIT(VulkanRenderContextTool)

        VulkanRenderContextTool(ImRoot &root);

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        std::string_view key() const override;

    private:
        Render::VulkanTexture mImageTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::VulkanRenderContextTool)