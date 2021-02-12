#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/threading/taskguard.h"

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/indexref.h"

#include "Meta/serialize/virtualserializableunit.h"

DECLARE_UNIQUE_COMPONENT(Engine::NodeGraph, Node, NodeBase, NodeGraph &);

namespace Engine {
namespace NodeGraph {

    template <typename T>
    struct Node : Serialize::VirtualData<T, VirtualScope<T, NamedComponent<T, NodeComponent<T>>>> {
        using Serialize::VirtualData<T, VirtualScope<T, NamedComponent<T, NodeComponent<T>>>>::VirtualData;

        virtual std::string_view name() const override {
            return this->componentName();
        }
    };

    MADGINE_NODEGRAPH_EXPORT std::map<std::string_view, IndexRef> &sNodesByName();

#define REGISTER_NODE(Name, target) \
    Engine::Threading::TaskGuard __##Name##_guard { []() { Engine::NodeGraph::sNodesByName()[#Name] = target; }, []() { Engine::NodeGraph::sNodesByName().erase(#Name); } };

#define NODE(Name, FullType)  \
    NAMED_UNIQUECOMPONENT(Name, FullType) \
    REGISTER_NODE(Name, Engine::indexRef<FullType>())

}
}