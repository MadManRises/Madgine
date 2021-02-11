#pragma once

#include "Meta/math/matrix4.h"

namespace Engine {

namespace Im3D {

    struct MeshParameters {

        constexpr MeshParameters() = default;

        constexpr MeshParameters(const Engine::Matrix4 &transform)
            : mTransform(transform)
        {
        }

        constexpr MeshParameters(const std::chrono::microseconds &duration)
            : mDuration(duration)
        {
        }
        Engine::Matrix4 mTransform = Engine::Matrix4::IDENTITY;
        std::chrono::microseconds mDuration = {};
    };

    struct Parameters : MeshParameters {

        constexpr Parameters() = default;

        constexpr Parameters(const Engine::Vector4 &color)
            : mColor(color)
        {
        }

        constexpr Parameters(const Engine::Matrix4 &transform)
            : MeshParameters(transform)
        {
        }

        constexpr Parameters(const std::chrono::microseconds &duration)
            : MeshParameters(duration)
        {
        }

        Engine::Vector4 mColor = { 1, 1, 1, 1 };
    };

    struct LineParameters : Parameters {

        constexpr LineParameters() = default;

        constexpr LineParameters(const Engine::Vector4 &color)
            : Parameters(color)
            , mColorB(color)
        {
        }

        constexpr LineParameters(const Engine::Vector4 &colorA, const Engine::Vector4 &colorB)
            : Parameters(colorA)
            , mColorB(colorB)
        {
        }

        Engine::Vector4 mColorB = { 1, 1, 1, 1 };
    };

    struct SphereParameters : Parameters {

        constexpr SphereParameters() = default;

        constexpr SphereParameters(size_t detail)
            : mDetail(detail)
        {
        }

        constexpr SphereParameters(const Engine::Vector4 &color, size_t detail)
            : Parameters(color)
            , mDetail(detail)
        {
        }

        size_t mDetail = 2;
    };

    struct TextParameters : Parameters {

        constexpr TextParameters(const Engine::Matrix4 &transform)
            : Parameters(transform)
        {
        }

        constexpr TextParameters(const Engine::Matrix4 &transform, float fontSize)
            : Parameters(transform), mFontSize(fontSize)
        {
        }

        float mFontSize = 24;
        bool mFacingX = true;
        bool mFacingY = true;
        const char *mFontName = "OpenSans-Regular";
        Vector2 mPivot = { 0.5f, 0.5f };
    };

}

}