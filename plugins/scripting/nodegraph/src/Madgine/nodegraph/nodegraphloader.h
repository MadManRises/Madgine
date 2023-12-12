#pragma once

#include "Madgine/resources/resourceloader.h"

#include "nodegraph.h"

#include "Madgine/behaviorcollector.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraphLoader : Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>> {

        using Base = Resources::ResourceLoader<NodeGraphLoader, NodeGraph, std::list<Placeholder<0>>>;

        struct Handle : Base::Handle {
            using Base::Handle::Handle;
            Handle(Base::Handle handle)
                : Base::Handle(std::move(handle))
            {
            }

            NodeInterpreter interpret();
        };

        NodeGraphLoader();

        bool loadImpl(NodeGraph &graph, ResourceDataInfo &info);
        void unloadImpl(NodeGraph &graph);

    };

    struct NodeGraphBehaviorFactory : BehaviorListComponent<NodeGraphBehaviorFactory> {
        static std::vector<std::string_view> names();
        static Behavior create(std::string_view name);
    };

}
}


DECLARE_BEHAVIOR_FACTORY(Engine::NodeGraph::NodeGraphBehaviorFactory)


REGISTER_TYPE(Engine::NodeGraph::NodeGraphLoader)