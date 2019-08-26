#pragma once

#include "objectptr.h"

namespace Engine {

struct MODULES_EXPORT NativeObject : ObjectPtr {

    NativeObject(std::map<std::string, ValueType> data);

};

}