#include "entitycomponenthandle.h"

#include "Modules/serialize/streams/operations.h"

#include "entity.h"

namespace Engine {
namespace Serialize {

    template <typename... Configs>
    struct Operations<std::pair<const uint32_t, Scene::Entity::EntityComponentOwningHandle<Scene::Entity::EntityComponentBase>>, Configs...> {

        static void read(SerializeInStream &in, std::pair<const uint32_t, Scene::Entity::EntityComponentOwningHandle<Scene::Entity::EntityComponentBase>> &p, const char *name, Scene::Entity::Entity *entity)
        {
            Serialize::read(in, *entity->sceneMgr().entityComponentList(p.first).get(p.second.mIndex), name, entity);
        }

        static void write(SerializeOutStream &out, const std::pair<const uint32_t, Scene::Entity::EntityComponentOwningHandle<Scene::Entity::EntityComponentBase>> &p, const char *name, const Scene::Entity::Entity *entity)
        {
            Serialize::write(out, *entity->sceneMgr().entityComponentList(p.first).get(p.second.mIndex), name, entity);
        }
    };

}
}