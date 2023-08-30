#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/rendercontext.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/scene/entity/components/pointlight.h"

#include "Madgine/scene/entity/components/material.h"

#include "Meta/math/transformation.h"

#include "scenemainwindowcomponent.h"

#include "Madgine/render/shadinglanguage/sl_support_begin.h"
#include "shaders/scene.sl"
#include "Madgine/render/shadinglanguage/sl_support_end.h"


#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::SceneRenderPass)
MEMBER(mAmbientFactor)
MEMBER(mDiffuseFactor)
MEMBER(mSpecularFactor)
MEMBER(mLightConstantFactor)
MEMBER(mLightLinearFactor)
MEMBER(mLightSquaredFactor)
METATABLE_END(Engine::Render::SceneRenderPass)

namespace Engine {
namespace Render {

    SceneRenderPass::SceneRenderPass(SceneMainWindowComponent &scene, Camera *camera, int priority)
        : mData(scene, camera)
        , mPriority(priority)
        , mShadowPass(scene, camera, 50)
    {
    }

    SceneRenderPass::SceneRenderPass(SceneRenderPass &&) = default;

    SceneRenderPass::~SceneRenderPass() = default;

    void SceneRenderPass::setup(RenderTarget *target)
    {
        mShadowMap = target->context()->createRenderTexture({ 2048, 2048 }, { .mName = "ShadowMap", .mType = TextureType_2DMultiSample, .mCreateDepthBufferView = true, .mSamples = 4, .mTextureCount = 0 });
        
        mShadowMap->addRenderPass(&mShadowPass);

        mPipeline.create({ .vs = "scene", .ps = "scene", .bufferSizes = { sizeof(ScenePerApplication), sizeof(ScenePerFrame), sizeof(ScenePerObject) }, .instanceDataSize = sizeof(SceneInstanceData) });

        addDependency(&mData);
        addDependency(mShadowMap.get());
        addDependency(mData.mScene.pointShadowTarget(0));
        addDependency(mData.mScene.pointShadowTarget(1));
        addDependency(mData.mScene.data());
    }

    void SceneRenderPass::shutdown()
    {
        removeDependency(&mData);
        removeDependency(mShadowMap.get());
        removeDependency(mData.mScene.pointShadowTarget(0));
        removeDependency(mData.mScene.pointShadowTarget(1));
        removeDependency(mData.mScene.data());

        mPipeline.reset();

        mShadowMap.reset();
    }

    void SceneRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {
        if (!mPipeline.available())
            return;

        target->pushAnnotation("Scene");

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        {
            auto perApplication = mPipeline->mapParameters<ScenePerApplication>(0);

            perApplication->p = target->getClipSpaceMatrix() * mData.mCamera->getProjectionMatrix(aspectRatio);

            perApplication->hasHDR = target->textureCount() > 1;

            perApplication->ambientFactor = mAmbientFactor;
            perApplication->diffuseFactor = mDiffuseFactor;
            perApplication->specularFactor = mSpecularFactor;
        }

        Matrix4 v = mData.mCamera->getViewMatrix();

        {
            auto perFrame = mPipeline->mapParameters<ScenePerFrame>(1);

            perFrame->light.caster.reprojectionMatrix = mShadowPass.viewProjectionMatrix() * v.Inverse();

            perFrame->light.caster.shadowSamples = 4;

            perFrame->light.light.color = mData.mScene.mAmbientLightColor;
            perFrame->light.light.dir = (v * Vector4 { mData.mScene.mAmbientLightDirection, 0.0f }).xyz();

            Scene::Entity::EntityComponentList<Scene::Entity::PointLight> &lights = mData.mScene.scene()->entityComponentList<Scene::Entity::PointLight>();
            perFrame->pointLightCount = lights.size();
            if (perFrame->pointLightCount > 2) {
                LOG_WARNING("Too many point lights in scene!");
                perFrame->pointLightCount = 2;
            }

            for (size_t i = 0; i < perFrame->pointLightCount; ++i) {
                Scene::Entity::Transform *t = lights.getEntity(i)->getComponent<Scene::Entity::Transform>();
                if (t) {
                    float range = lights[i].mRange;
                    perFrame->pointLights[i].light.position = (v * Vector4 { t->mPosition, 1.0f }).xyz();
                    perFrame->pointLights[i].light.color = lights[i].mColor;
                    perFrame->pointLights[i].light.constantFactor = mLightConstantFactor;
                    perFrame->pointLights[i].light.linearFactor = mLightLinearFactor / range;
                    perFrame->pointLights[i].light.squaredFactor = mLightSquaredFactor / (range * range);
                    perFrame->pointLights[i].caster.reprojectionMatrix = v.Inverse();
                }
            }
        }

        mPipeline->bindTextures(target, { mShadowMap->depthTexture() }, 2);
        mPipeline->bindTextures(target, mData.mScene.depthTextures(), 3);

        for (const std::pair<const std::tuple<const GPUMeshData *, const GPUMeshData::Material *>, std::vector<LitSceneRenderData::ObjectData>> &instance : mData.mInstances) {
            const GPUMeshData *meshData = std::get<0>(instance.first);
            const GPUMeshData::Material *material = std::get<1>(instance.first);            

            {
                auto perObject = mPipeline->mapParameters<ScenePerObject>(2);

                perObject->hasLight = true;

                perObject->hasDistanceField = false;

                perObject->hasTexture = material && material->mDiffuseTexture.available() && material->mDiffuseTexture->handle() != 0;

                perObject->diffuseColor = material ? material->mDiffuseColor : Vector4::UNIT_SCALE;

                perObject->specularColor = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f };

                perObject->shininess = 32.0f;
            }

            mPipeline->bindTextures(target, { material && material->mDiffuseTexture.available() ? material->mDiffuseTexture->descriptor() : TextureDescriptor {}, material && material->mEmissiveTexture.available() ? material->mEmissiveTexture->descriptor() : TextureDescriptor {} });

            std::vector<SceneInstanceData> instanceData;

            std::ranges::transform(instance.second, std::back_inserter(instanceData), [&](const LitSceneRenderData::ObjectData &o) {
                Matrix4 mv = v * o.mTransform;
                return SceneInstanceData {
                    mv.Transpose(),
                    mv.Inverse() /*.Transpose().Transpose()*/,
                    o.mBones
                };
            });

            mPipeline->renderMeshInstanced(target, std::move(instanceData), meshData);
        }
        target->popAnnotation();
    }

    int SceneRenderPass::priority() const
    {
        return mPriority;
    }

    void SceneRenderPass::debugCameras(Lambda<void(const Camera &, std::string_view)> handler) const
    {
        handler(*mData.mCamera, "SceneRenderPass");
    }

}
}
