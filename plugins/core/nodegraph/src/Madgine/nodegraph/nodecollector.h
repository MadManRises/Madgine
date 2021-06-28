#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/threading/taskguard.h"

#include "Modules/uniquecomponent/indexref.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Meta/serialize/virtualserializableunit.h"

DECLARE_UNIQUE_COMPONENT(Engine::NodeGraph, Node, NodeBase, NodeGraph &);

namespace Engine {
namespace NodeGraph {

    template <typename T, typename Base = NodeBase>
    struct Node : Serialize::VirtualData<T, VirtualScope<T, NamedComponent<T, NodeComponent<T, Base>>>> {
        using Serialize::VirtualData<T, VirtualScope<T, NamedComponent<T, NodeComponent<T, Base>>>>::VirtualData;

        virtual std::string_view className() const override final
        {
            return this->componentName();
        }
        virtual std::string_view name() const override
        {
            return className();
        }
        virtual std::unique_ptr<NodeBase> clone(NodeGraph& graph) const override {
            return std::make_unique<T>(*static_cast<const T *>(this), graph);
        }
    };

    MADGINE_NODEGRAPH_EXPORT std::map<std::string_view, IndexRef> &sNodesByName();

#define REGISTER_NODE(Name, target) \
    Engine::Threading::TaskGuard __##Name##_guard { []() { Engine::NodeGraph::sNodesByName()[#Name] = target; }, []() { Engine::NodeGraph::sNodesByName().erase(#Name); } };

#define NODE(Name, FullType)              \
    NAMED_UNIQUECOMPONENT(Name, FullType) \
    REGISTER_NODE(Name, Engine::indexRef<FullType>())

}
}