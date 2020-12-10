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

        struct EntityPtr;
        template <typename T>
        struct EntityComponentPtr;
        template <typename T>
        struct EntityComponentList;
        template <typename T>
        struct EntityComponentOwningHandle;
    }

    struct LightManager;
    struct Light;
}

}
