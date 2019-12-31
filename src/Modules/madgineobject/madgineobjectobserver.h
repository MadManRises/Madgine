#pragma once

#include "../generic/observerevent.h"

namespace Engine {

struct MODULES_EXPORT MadgineObjectObserver {

    void handle(MadgineObject *object, int event) const;

    template <typename It>
    void operator()(const It &it, int event) const
    {
        if ((event == (AFTER | INSERT_ITEM)) || (event == (BEFORE | REMOVE_ITEM))) {
            handle(&**it, event);
        } else {
            handle(nullptr, event);
        }
    }
};
}