
#define NAMED(Name, Type)                                 \
    struct field_Name_##Name {                            \
        static constexpr const char *sName = #Name; \
    };                                                    \
    Engine::Render::ShadingLanguage::Named<Type, field_Name_##Name> Name

#define parameters struct alignas(16)

#define float3(Name) NAMED(Name, Engine::Vector3)
#define float4x4(Name) NAMED(Name, Engine::Matrix4)
#define bool(Name) NAMED(Name, uint32_t)

#include STRINGIFY2(SL_SHADER.sl)

#undef NAMED

#undef parameters

#undef float3
#undef float4x4
#undef bool

#undef SL_SHADER
