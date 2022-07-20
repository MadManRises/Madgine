#pragma once

#include "Madgine_Tools/toolscollector.h"

#include "Madgine_Tools/toolbase.h"

#include "LinearMath/btIDebugDraw.h"

namespace Engine {
namespace Tools {

    struct BulletDebug : Tool<BulletDebug>, btIDebugDraw {

        SERIALIZABLEUNIT(BulletDebug)

        BulletDebug(ImRoot &root);
        BulletDebug(const BulletDebug &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        std::string_view key() const override;

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor) override;

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

        virtual void drawSphere(const btVector3 &p, btScalar radius, const btVector3 &color) override;

        virtual void drawTriangle(const btVector3 &a, const btVector3 &b, const btVector3 &c, const btVector3 &color, btScalar alpha) override;

        virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;

        virtual void reportErrorWarning(const char *warningString) override;

        virtual void draw3dText(const btVector3 &location, const char *textString) override;

        virtual void setDebugMode(int debugMode) override;

        virtual int getDebugMode() const override { return mDebugMode; }

    private:
        unsigned int mDebugMode;

        Physics::PhysicsManager *mManager;
    };

}
}

REGISTER_TYPE(Engine::Tools::BulletDebug)