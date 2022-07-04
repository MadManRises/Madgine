#pragma once

#include "../entitycomponent.h"

#include "animationloader.h"
#include "skeletonloader.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT Animation : EntityComponent<Animation> {
            using EntityComponent<Animation>::EntityComponent;
            virtual ~Animation() = default;

            void set(Render::AnimationLoader::Handle handle);
            void setName(const std::string &name);
            Render::AnimationLoader::Resource *get() const;

            void setCurrentAnimationName(const std::string &name);
            void setCurrentAnimation(Render::AnimationDescriptor *desc);
            Render::AnimationDescriptor *currentAnimation() const;

            void step(float delta);
            void setStep(float step);
            float currentStep() const;

            void updateRender(Entity *entity);

        private:
            void refreshCache();

        private:
            Render::AnimationLoader::Handle mAnimationList;
            Render::SkeletonLoader::Handle mSkeletonCache;
            int *mBoneIndexMapping = nullptr;
            Render::AnimationDescriptor *mCurrentAnimation = nullptr;
            float mCurrentStep = 0.0f;
        };

        using AnimationPtr = EntityComponentPtr<Animation>;

    }
}
}

REGISTER_TYPE(Engine::Scene::Entity::Animation)
REGISTER_TYPE(Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>)