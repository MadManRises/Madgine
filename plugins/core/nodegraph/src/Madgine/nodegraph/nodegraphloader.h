#pragma once

#include "Madgine/resources/resourceloader.h"

#include "nodegraph.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraphLoader : Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>> {

        using Base = Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>>;

        struct MADGINE_NODEGRAPH_EXPORT HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            Threading::TaskFuture<bool> create(const Filesystem::Path &path);
        };

        NodeGraphLoader();

        bool loadImpl(NodeGraph &graph, ResourceDataInfo &info);
        void unloadImpl(NodeGraph &graph, ResourceDataInfo &info);

    };

}
}

RegisterType(Engine::NodeGraph::NodeGraphLoader);