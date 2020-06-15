#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/threading/taskguard.h"

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/indexref.h"

DECLARE_UNIQUE_COMPONENT(Engine::NodeGraph, NodePrototype, NodePrototypeBase);

namespace Engine {
namespace NodeGraph {

    template <typename T>
    struct NodePrototype : NamedComponent<T, NodePrototypeComponent<T>> {
        using NamedComponent<T, NodePrototypeComponent<T>>::NamedComponent;

        virtual const std::string_view& name() const override {
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