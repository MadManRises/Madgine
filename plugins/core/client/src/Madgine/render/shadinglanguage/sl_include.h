
#include "Meta/math/matrix4.h"

#define struct struct alignas(16)

#define int2 alignas(8) Engine::Vector2i
#define float3 alignas(16) Engine::Vector3
#define float4 alignas(16) Engine::Vector4
#define float4x4 alignas(16) Engine::Matrix4
#define bool uint32_t

#include STRINGIFY2(SL_SHADER.sl)

#undef NAMED

#undef struct

#undef int2
#undef float3
#undef float4
#undef float4x4
#undef bool

#undef SL_SHADER
