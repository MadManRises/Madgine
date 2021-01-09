#pragma once

#include "Modules/math/quaternion.h"
#include "Modules/math/vector3.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT Camera : ScopeBase {
        Camera(std::string name = "");
        //Camera(const Camera &) = delete;
        ~Camera() = default;

        Matrix4 getViewProjectionMatrix(float aspectRatio);
        Matrix4 getViewMatrix();
        Matrix4 getProjectionMatrix(float aspectRatio);

        Ray mousePointToRay(const Vector2 &mousePos, const Vector2 &viewportSize);
        Ray toRay() const;

        std::string mName;

        Vector3 mPosition;
        Quaternion mOrientation;

        float mF;
        float mN;
        float mFOV;
    };

}
}

RegisterType(Engine::Render::Camera);