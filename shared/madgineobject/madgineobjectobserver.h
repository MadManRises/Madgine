#pragma once

#include "Generic/container/containerevent.h"

namespace Engine {

struct MadgineObjectObserver {

    template <typename T>
    void handle(T *object, int event) const
    {
        switch (event) {
        case AFTER | EMPLACE:
            if constexpr (T::hasParent()) {
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