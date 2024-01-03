#include "../nodegraphlib.h"

#include "nodegraphloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/streams/streamresult.h"

#include "nodeinterpreter.h"

UNIQUECOMPONENT(Engine::NodeGraph::NodeGraphLoader)

METATABLE_BEGIN(Engine::NodeGraph::NodeGraphLoader)
MEMBER(mResources)
METATABLE_END(Engine::NodeGraph::NodeGraphLoader)

METATABLE_BEGIN_BASE(Engine::NodeGraph::NodeGraphLoader::Resource, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::NodeGraph::NodeGraphLoader::Resource)

DEFINE_BEHAVIOR_FACTORY(NodeGraph, Engine::NodeGraph::NodeGraphBehaviorFactory)

namespace Engine {
namespace NodeGraph {

    NodeInterpreter NodeGraphLoader::Handle::interpret()
    {
        return { *this };
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

    std::vector<std::string_view> NodeGraphBehaviorFactory::names()
    {
        const auto &names = NodeGraphLoader::getSingleton().resources() | std::ranges::views::transform([](Resources::ResourceBase *resource) { return resource->name(); });
        return { names.begin(), names.end() };
    }

    Behavior NodeGraphBehaviorFactory::create(std::string_view name)
    {
        return NodeGraphLoader::Handle { NodeGraphLoader::load(name) }.interpret();
    }

}
}

