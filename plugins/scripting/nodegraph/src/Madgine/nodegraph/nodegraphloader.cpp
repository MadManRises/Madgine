#include "../nodegraphlib.h"

#include "nodegraphloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/streams/streamresult.h"

UNIQUECOMPONENT(Engine::NodeGraph::NodeGraphLoader)

METATABLE_BEGIN(Engine::NodeGraph::NodeGraphLoader)
MEMBER(mResources)
METATABLE_END(Engine::NodeGraph::NodeGraphLoader)

METATABLE_BEGIN_BASE(Engine::NodeGraph::NodeGraphLoader::Resource, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::NodeGraph::NodeGraphLoader::Resource)



    namespace Engine
{
    namespace NodeGraph {

        NodeGraphLoader::NodeGraphLoader()
            : ResourceLoader({ ".ngp" }, { .mAutoReload = true })
        {
        }

        bool NodeGraphLoader::loadImpl(NodeGraph &graph, ResourceDataInfo &info)
        {   
            Serialize::StreamResult result = graph.loadFromFile(info.resource()->path());

            if (result.mState != Serialize::StreamState::OK) {
                LOG_ERROR("Error loading Nodegraph (" << info.resource()->path() << "):\n" << result);
                return false;
            }

            return true;
        }

        void NodeGraphLoader::unloadImpl(NodeGraph &graph)
        {
        }

    }
}