#pragma once

#include "behaviorlib.h"

#include "behaviorreceiver.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {

bool BehaviorReceiver::resolveVarHelper(std::string_view name, void (*f)(const ValueType &, void *), void *data)
{
    ValueType v;
    if (resolveVar(name, v)) {
        f(v, data);
        return true;
    } else {
        return false;
    }
}

}