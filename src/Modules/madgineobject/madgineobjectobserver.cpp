#include "../moduleslib.h"

#include "madgineobjectobserver.h"

#include "madgineobject.h"

#include "../generic/container/observerevent.h"

namespace Engine {

void MadgineObjectObserver::handle(MadgineObject *object, int event) const
{
    switch (event) {
    case AFTER | INSERT_ITEM:
        if (!object->parent() || object->parent()->isInitialized())
			object->callInit();
        break;
    case BEFORE | REMOVE_ITEM:
        object->callFinalize();
        break;
    }
}

}