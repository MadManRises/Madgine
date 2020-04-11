#include "../nodegraphlib.h"

#include "nodegraphprototypeloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

UNIQUECOMPONENT(Engine::NodeGraph::NodeGraphPrototypeLoader)

METATABLE_BEGIN(Engine::NodeGraph::NodeGraphPrototypeLoader)
MEMBER(mResources)
METATABLE_END(Engine::NodeGraph::NodeGraphPrototypeLoader)

METATABLE_BEGIN_BASE(Engine::NodeGraph::NodeGraphPrototypeLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::NodeGraph::NodeGraphPrototypeLoader::ResourceType)

RegisterType(Engine::NodeGraph::NodeGraphPrototypeLoader)

    namespace Engine
{
    namespace NodeGraph {

        void NodeGraphPrototypeLoader::HandleType::create(const Filesystem::Path &path)
        {
            *this = NodeGraphPrototypeLoader::loadManual(path.stem(), path);
        }

        NodeGraphPrototypeLoader::NodeGraphPrototypeLoader()
            : ResourceLoader({ ".ngp" })
        {
        }

        bool NodeGraphPrototypeLoader::loadImpl(Serialize::NoParentUnit<NodeGraphPrototype> &graph, ResourceType *res)
        {   
            graph.loadFromFile(res->path());

            return true;
        }

        void NodeGraphPrototypeLoader::unloadImpl(Serialize::NoParentUnit<NodeGraphPrototype> &graph, ResourceType *res)
        {
        }

    }
}