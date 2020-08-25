#pragma once

#include "toolscollector.h"

#include "toolbase.h"

#include "LinearMath/btIDebugDraw.h"

namespace Engine {
namespace Tools {

    struct BulletDebug : Tool<BulletDebug>, btIDebugDraw {

        SERIALIZABLEUNIT;

        BulletDebug(ImRoot &root);
        BulletDebug(const BulletDebug &) = delete;

        virtual bool init() override;
        virtual void finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        std::string_view key() const override;


        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor);

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);

        virtual void drawSphere(const btVector3 &p, btScalar radius, const btVector3 &color);

        virtual void drawTriangle(const btVector3 &a, const btVector3 &b, const btVector3 &c, const btVector3 &color, btScalar alpha);

        virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);

        virtual void reportErrorWarning(const char *warningString);

        virtual void draw3dText(const btVector3 &location, const char *textString);

        virtual void setDebugMode(int debugMode);

        virtual int getDebugMode() const { return mDebugMode; }

    private:
        unsigned int mDebugMode;

        Physics::PhysicsManager *mManager;
    };

}
}

RegisterType(Engine::Tools::BulletDebug);