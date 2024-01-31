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

            NodeInterpreterSender interpret();
        };

        NodeGraphLoader();

        bool loadImpl(NodeGraph &graph, ResourceDataInfo &info);
        void unloadImpl(NodeGraph &graph);

    };

    struct NodeGraphBehaviorFactory : BehaviorFactory<NodeGraphBehaviorFactory> {
        std::vector<std::string_view> names() const override;
        Behavior create(std::string_view name, const ParameterTuple &args) const override;
        Threading::TaskFuture<ParameterTuple> createParameters(std::string_view name) const override;
        bool isConstant(std::string_view name) const override;
        std::vector<ValueTypeDesc> parameterTypes(std::string_view name) const override;
        std::vector<ValueTypeDesc> resultTypes(std::string_view name) const override;  
    };

}
}


DECLARE_BEHAVIOR_FACTORY(Engine::NodeGraph::NodeGraphBehaviorFactory)


REGISTER_TYPE(Engine::NodeGraph::NodeGraphLoader)