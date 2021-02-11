#pragma once

#include "Generic/type_pack.h"

namespace Engine {

using ValueTypeList = type_pack<
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Name, Storage, ...) type_pack_apply_t<std::decay_t, type_pack<__VA_ARGS__>>
#include "valuetypedef.h"
    >;

using QualifiedValueTypeList = type_pack<
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Name, Storage, ...) type_pack<__VA_ARGS__>
#include "valuetypedef.h"
    >;

}