#pragma once

#include "../generic/container/containerevent.h"

namespace Engine {

struct MODULES_EXPORT MadgineObjectObserver {

    void handle(MadgineObject *object, int event) const;

    template <typename It>
    void operator()(const It &it, int event) const
    {
        if ((event == (AFTER | EMPLACE)) || (event == (BEFORE | ERASE))) {
            handle(&**it, event);
        } else {
            handle(nullptr, event);
        }
    }
};
}