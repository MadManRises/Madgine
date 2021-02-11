#pragma once

#include "valuetype.h"

namespace Engine {

struct KeyValuePair {
    ValueType mKey;
    ValueTypeRef mValue;
};

}