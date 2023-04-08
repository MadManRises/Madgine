#pragma once

#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"

namespace Engine {
namespace Tools {

    struct PluginExporter : Root::RootComponent<PluginExporter> {

        PluginExporter(Root::Root &root);

        static void exportStaticComponentHeader(const Filesystem::Path &outFile);

        virtual std::string_view key() const override;
    };

}
}

REGISTER_TYPE(Engine::Tools::PluginExporter)