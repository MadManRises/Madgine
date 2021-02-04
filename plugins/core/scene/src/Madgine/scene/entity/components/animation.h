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

            void set(const Render::AnimationLoader::HandleType &handle);
            void setName(const std::string &name);
            Render::AnimationLoader::ResourceType *get() const;

            void setCurrentAnimationName(const std::string &name);
            void setCurrentAnimation(Render::AnimationDescriptor *desc);
            Render::AnimationDescriptor *currentAnimation() const;

            void step(float delta);
            void setStep(float step);
            float currentStep() const;

            void applyTransform(const EntityPtr &entity);

        private:
            void refreshCache();

        private:
            Render::AnimationLoader::HandleType mAnimationList;
            Render::SkeletonLoader::HandleType mSkeletonCache;
            int *mBoneIndexMapping = nullptr;
            Render::AnimationDescriptor *mCurrentAnimation = nullptr;
            float mCurrentStep = 0.0f;
        };

        using AnimationPtr = EntityComponentPtr<Animation>;

    }
}
}

RegisterType(Engine::Scene::Entity::Animation);
RegisterType(Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>);