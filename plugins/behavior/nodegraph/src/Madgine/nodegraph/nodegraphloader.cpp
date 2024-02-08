#include "../nodegraphlib.h"

#include "nodegraphloader.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/serialize/streams/streamresult.h"

#include "nodeinterpreter.h"

#include "Madgine/parametertuple.h"

RESOURCELOADER(Engine::NodeGraph::NodeGraphLoader)

DEFINE_BEHAVIOR_FACTORY(NodeGraph, Engine::NodeGraph::NodeGraphBehaviorFactory)

namespace Engine {
namespace NodeGraph {

    Resources::with_handle_t::sender<NodeInterpreterSender, NodeGraphLoader::Handle> NodeGraphLoader::Handle::interpret()
    {
        return NodeInterpreterSender { *this } | Resources::with_handle(Handle { *this });
    }

    NodeGraphLoader::NodeGraphLoader()
        : ResourceLoader({ ".ngp" }, { .mAutoReload = true })
    {
    }

    bool NodeGraphLoader::loadImpl(NodeGraph &graph, ResourceDataInfo &info)
    {
        Serialize::StreamResult result = graph.loadFromFile(info.resource()->path());

        if (result.mState != Serialize::StreamState::OK) {
            LOG_ERROR("Error loading Nodegraph (" << info.resource()->path() << "):\n"
                                                  << result);
            return false;
        }

        return true;
    }

    void NodeGraphLoader::unloadImpl(NodeGraph &graph)
    {
    }

    std::vector<std::string_view> NodeGraphBehaviorFactory::names() const
    {
        const auto &names = NodeGraphLoader::getSingleton().resources() | std::ranges::views::transform([](Resources::ResourceBase *resource) { return resource->name(); });
        return { names.begin(), names.end() };
    }

    Behavior NodeGraphBehaviorFactory::create(std::string_view name, const ParameterTuple &args) const
    {
        return NodeGraphLoader::Handle { NodeGraphLoader::load(name) }.interpret();
    }

    Threading::TaskFuture<ParameterTuple> NodeGraphBehaviorFactory::createParameters(std::string_view name) const
    {
        return ParameterTuple { std::make_tuple() };
    }

    bool NodeGraphBehaviorFactory::isConstant(std::string_view name) const
    {
        return false;
    }

    std::vector<ValueTypeDesc> NodeGraphBehaviorFactory::parameterTypes(std::string_view name) const
    {
        return {};
    }

    std::vector<ValueTypeDesc> NodeGraphBehaviorFactory::resultTypes(std::string_view name) const
    {
        return {};
    }

}
}
