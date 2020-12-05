#include "../../scenelib.h"

#include "entitycomponentlist.h"

#include "../scenemanager.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityHandle copyEntityHandle(SceneManager *mgr, const EntityHandle &entity)
        {
            return mgr->copyEntityHandle(entity);
        }

    }
}
}
