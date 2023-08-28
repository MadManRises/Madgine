#include "../../scenelib.h"

#include "entitybehavior.h"

#include "entity.h"

#include "../scenemanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Scene::Entity::EntityBehavior)
READONLY_PROPERTY(Graph, get)
READONLY_PROPERTY(State, variables)
METATABLE_END(Engine::Scene::Entity::EntityBehavior)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::EntityBehavior)
SERIALIZETABLE_END(Engine::Scene::Entity::EntityBehavior)

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityBehavior::EntityBehavior(NodeGraph::NodeGraphLoader::Resource *graph, Entity *entity)
            : mGraph(graph)
        {
            mGraph.info()->loadingTask().then([=](bool result) {if (result)
                mState.setGraph(&*mGraph); }, entity->sceneMgr().taskQueue());
        }

        EntityBehavior::EntityBehavior(std::string_view name, Entity *entity)
        {
            mGraph.load(name).then([=](bool result) {if (result)
                mState.setGraph(&*mGraph); }, entity->sceneMgr().taskQueue());
        }

        std::string_view EntityBehavior::getName() const
        {
            return mGraph.name();
        }

        NodeGraph::NodeGraphLoader::Resource *EntityBehavior::get() const
        {
            return mGraph.resource();
        }

        std::map<std::string_view, ValueType> EntityBehavior::variables()
        {
            return mState.variables();
        }

    }
}
}