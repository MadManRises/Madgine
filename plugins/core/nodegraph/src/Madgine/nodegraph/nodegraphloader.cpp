#include "../nodegraphlib.h"

#include "nodegraphloader.h"

#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::NodeGraph::NodeGraphLoader)

METATABLE_BEGIN(Engine::NodeGraph::NodeGraphLoader)
MEMBER(mResources)
METATABLE_END(Engine::NodeGraph::NodeGraphLoader)

METATABLE_BEGIN_BASE(Engine::NodeGraph::NodeGraphLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::NodeGraph::NodeGraphLoader::ResourceType)



    namespace Engine
{
    namespace NodeGraph {

        void NodeGraphLoader::HandleType::create(const Filesystem::Path &path)
        {
            *this = NodeGraphLoader::loadManual(path.stem(), path);
        }

        NodeGraphLoader::NodeGraphLoader()
            : ResourceLoader({ ".ngp" })
        {
        }

        bool NodeGraphLoader::loadImpl(Serialize::NoParentUnit<NodeGraph> &graph, ResourceType *res)
        {   
            graph.loadFromFile(res->path());

            return true;
        }

        void NodeGraphLoader::unloadImpl(Serialize::NoParentUnit<NodeGraph> &graph, ResourceType *res)
        {
        }

    }
}