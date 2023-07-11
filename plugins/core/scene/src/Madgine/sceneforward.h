#pragma once

namespace Engine {
namespace Scene {
    struct SceneManager;
    struct SceneComponentBase;

    namespace Entity {
        struct Entity;
        struct EntityComponentBase;
        struct EntityComponentListBase;
        struct Transform;
        struct Animation;
        struct Mesh;
        struct Skeleton;

        struct EntityPtr;
        template <typename T>
        struct EntityComponentPtr;
        template <typename T>
        struct EntityComponentList;
        template <typename T>
        struct EntityComponentOwningHandle;
        template <typename T>
        struct EntityComponentHandle;
    }

    struct LightManager;
    struct Light;
}

}
