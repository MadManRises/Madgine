#pragma once

#include "Madgine/resources/resourceloader.h"

#include "nodegraph.h"

#include "Meta/serialize/container/noparent.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraphLoader : Resources::ResourceLoader<NodeGraphLoader, Serialize::NoParentUnit<NodeGraph>, std::list<Placeholder<0>>> {

        using Base = Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>>;

        struct MADGINE_NODEGRAPH_EXPORT HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

            void create(const Filesystem::Path &path);
        };

        NodeGraphLoader();

        bool loadImpl(Serialize::NoParentUnit<NodeGraph> &graph, ResourceType *res);
        void unloadImpl(Serialize::NoParentUnit<NodeGraph> &graph, ResourceType *res);

    };

}
}

RegisterType(Engine::NodeGraph::NodeGraphLoader);