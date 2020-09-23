#pragma once

#include "../generic/container/containerevent.h"

namespace Engine {

struct MODULES_EXPORT MadgineObjectObserver {

    template <typename T>
    void handle(T *object, int event) const
    {
        switch (event) {
        case AFTER | EMPLACE:
            if constexpr (object->hasParent()) {
                if (object->getParent()->isInitialized())
                    object->callInit();
            } else {
                object->callInit();
            }
            break;
        case BEFORE | ERASE:
            object->callFinalize();
            break;
        }
    }

    template <typename It>
    void operator()(const It &it, int event) const
    {
        if ((event == (AFTER | EMPLACE)) || (event == (BEFORE | ERASE))) {
            handle(&**it, event);
        } else {
            //handle(static_cast<MadgineObject<MadgineObjectInterface>*>(nullptr), event);
        }
    }
};
}