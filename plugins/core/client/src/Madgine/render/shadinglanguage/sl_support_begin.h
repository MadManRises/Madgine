
#include "Meta/math/matrix4.h"
#include "Madgine/render/ptr.h"
#include "Madgine/render/aligned.h"

#define struct struct alignas(16)

#define int2 Engine::Render::Aligned<Engine::Vector2i, 8>
#define float2 Engine::Render::Aligned<Engine::Vector2, 8>
#define float3 Engine::Render::Aligned<Engine::Vector3, 16>
#define float4 Engine::Render::Aligned<Engine::Vector4, 16>
#define float4x4 Engine::Render::Aligned<Engine::Matrix4, 16>
#define bool uint32_t

#define row_major

#define ArrayPtr Engine::Render::GPUArrayPtr
