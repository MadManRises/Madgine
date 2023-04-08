#pragma once

#include "Madgine_Tools/toolbase.h"
#include "Madgine_Tools/toolscollector.h"

#include "Vulkan/util/vulkantexture.h"

namespace Engine {
namespace Tools {

    struct VulkanToolConfig : public Tool<VulkanToolConfig> {

        SERIALIZABLEUNIT(VulkanToolConfig)

        VulkanToolConfig(ImRoot &root);
        VulkanToolConfig(const SceneEditor &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void renderMenu() override;

        std::string_view key() const override;

    private:
        Render::VulkanTexture mImageTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::VulkanToolConfig)