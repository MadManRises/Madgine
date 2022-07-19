#pragma once

#include "toolbase.h"
#include "toolscollector.h"

namespace Engine {
namespace Tools {

    struct ResourcesToolConfig : public Tool<ResourcesToolConfig> {

        SERIALIZABLEUNIT(ResourcesToolConfig)

        ResourcesToolConfig(ImRoot &root);        

        virtual Threading::Task<bool> init() override;

        virtual void renderMenu() override;

        std::string_view key() const override;

    };

}
}

REGISTER_TYPE(Engine::Tools::ResourcesToolConfig)