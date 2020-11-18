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
        struct EntityHandle;
        template <typename T>
        struct EntityComponentPtr;
        template <typename T>
        struct EntityComponentList;
    }

    struct LightManager;
    struct Light;
}

}
