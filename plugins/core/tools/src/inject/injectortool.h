#pragma once
#include "../toolscollector.h"
#include "../toolbase.h"

#include "controlflow.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT InjectorTool : Tool<InjectorTool> {
        InjectorTool(ImRoot &root);
        InjectorTool(const InjectorTool &) = delete;
        ~InjectorTool();

        virtual void render() override;


        virtual std::string_view key() const override;
      
        Inject::ControlFlow::Graph mGraph;

    };
}
}

REGISTER_TYPE(Engine::Tools::InjectorTool)