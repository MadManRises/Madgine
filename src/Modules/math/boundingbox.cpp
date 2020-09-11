#include "../moduleslib.h"

#include "boundingbox.h"

#include "../keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::AABB)
MEMBER(mMin)
MEMBER(mMax)
METATABLE_END(Engine::AABB)