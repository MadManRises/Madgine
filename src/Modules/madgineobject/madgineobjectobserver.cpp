#include "../moduleslib.h"

#include "madgineobjectobserver.h"

#include "madgineobject.h"

#include "../generic/container/containerevent.h"

namespace Engine {

void MadgineObjectObserver::handle(MadgineObject *object, int event) const
{
    switch (event) {
    case AFTER | EMPLACE:
        if (!object->parent() || object->parent()->isInitialized())
			object->callInit();
        break;
    case BEFORE | ERASE:
        object->callFinalize();
        break;
    }
}

}