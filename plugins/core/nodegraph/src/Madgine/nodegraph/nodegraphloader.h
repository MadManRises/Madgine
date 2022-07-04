#pragma once

#include "Madgine/resources/resourceloader.h"

#include "nodegraph.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraphLoader : Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>> {

        using Base = Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>>;

        NodeGraphLoader();

        bool loadImpl(NodeGraph &graph, ResourceDataInfo &info);
        void unloadImpl(NodeGraph &graph);

    };

}
}

REGISTER_TYPE(Engine::NodeGraph::NodeGraphLoader)