#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Madgine/render/rendercontext.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/scene/entity/components/pointlight.h"

#include "Madgine/scene/entity/components/material.h"

#include "Meta/math/transformation.h"

#define SL_SHADER "shaders/scene.sl"
#include INCLUDE_SL_SHADER

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::SceneRenderPass)
MEMBER(mAmbientFactor)
MEMBER(mDiffuseFactor)
MEMBER(mSpecularFactor)
METATABLE_END(Engine::Render::SceneRenderPass)

namespace Engine {
namespace Render {

    SceneRenderPass::SceneRenderPass(Scene::SceneManager &scene, Camera *camera, int priority)
        : mShadowPass(scene, camera, 50)
        , mPointShadowPasses { { 0, scene, 50 }, { 1, scene, 50 } }
        , mScene(scene)
        , mCamera(camera)
        , mPriority(priority)
    {
    }

    SceneRenderPass::~SceneRenderPass() = default;

    void SceneRenderPass::setup(RenderTarget *target)
    {
        mPipeline.create({ .vs = "scene", .ps = "scene", .bufferSizes = { sizeof(ScenePerApplication), sizeof(ScenePerFrame), sizeof(ScenePerObject) }, .instanceDataSize = sizeof(SceneInstanceData) });

        mShadowMap = target->context()->createRenderTexture({ 2048, 2048 }, { .mCreateDepthBufferView = true, .mSamples = 4, .mTextureCount = 0, .mType = TextureType_2DMultiSample });
        mPointShadowMaps[0] = target->context()->createRenderTexture({ 2048, 2048 }, { .mCreateDepthBufferView = true, .mTextureCount = 0, .mType = TextureType_Cube });
        mPointShadowMaps[1] = target->context()->createRenderTexture({ 2048, 2048 }, { .mCreateDepthBufferView = true, .mTextureCount = 0, .mType = TextureType_Cube });

        mShadowMap->addRenderPass(&mShadowPass);
        mPointShadowMaps[0]->addRenderPass(&mPointShadowPasses[0]);
        mPointShadowMaps[1]->addRenderPass(&mPointShadowPasses[1]);
    }

    void SceneRenderPass::shutdown()
    {
        mShadowMap.reset();
        mPointShadowMaps[0].reset();
        mPointShadowMaps[1].reset();

        mPipeline.reset();
    }

    void SceneRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {
        if (!mPipeline.available())
            return;
        //TODO Culling

        std::map<std::tuple<const GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> instances;

        for (const auto &[mesh, e] : mScene.entityComponentList<Scene::Entity::Mesh>().data()) {
            if (!mesh.isVisible())
                continue;

            const GPUMeshData *meshData = mesh.data();
            if (!meshData)
                continue;

            Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
            if (!transform)
                continue;

            const GPUMeshData::Material *material = nullptr;
            Scene::Entity::Material *materialComponent = e->getComponent<Scene::Entity::Material>();
            if (materialComponent) {
                material = materialComponent->get();
            } else if (mesh.material() < meshData->mMaterials.size()) {
                material = &meshData->mMaterials[mesh.material()];
            }

            Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();

            instances[std::tuple<const GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *> { meshData, material, skeleton }].push_back(transform->worldMatrix());
        }

        target->pushAnnotation("Scene");

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        {
            auto perApplication = mPipeline->mapParameters<ScenePerApplication>(0);

            perApplication->p = target->getClipSpaceMatrix() * mCamera->getProjectionMatrix(aspectRatio);

            perApplication->hasHDR = target->textureCount() > 1;

            perApplication->ambientFactor = mAmbientFactor;
            perApplication->diffuseFactor = mDiffuseFactor;
            perApplication->specularFactor = mSpecularFactor;
        }

        Matrix4 v = mCamera->getViewMatrix();

        {
            auto perFrame = mPipeline->mapParameters<ScenePerFrame>(1);

            perFrame->light.caster.reprojectionMatrix = mShadowPass.viewProjectionMatrix() * v.Inverse();

            perFrame->light.caster.shadowSamples = 4;

            perFrame->light.light.color = mScene.mAmbientLightColor;
            perFrame->light.light.dir = (v * Vector4 { mScene.mAmbientLightDirection, 0.0f }).xyz();

            Scene::Entity::EntityComponentList<Scene::Entity::PointLight> &lights = mScene.entityComponentList<Scene::Entity::PointLight>();
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
                    perFrame->pointLights[i].light.constant = 1.0f;
                    perFrame->pointLights[i].light.linearFactor = 4.5f / range;
                    perFrame->pointLights[i].light.quadratic = 75.0f / (range * range);
                    perFrame->pointLights[i].caster.reprojectionMatrix = v.Inverse();
                }
            }
        }

        mPipeline->bindTextures({ mShadowMap->depthTexture(), mPointShadowMaps[0]->depthTexture(), mPointShadowMaps[1]->depthTexture() }, 2);

        for (const std::pair<const std::tuple<const GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> &instance : instances) {
            const GPUMeshData *meshData = std::get<0>(instance.first);
            const GPUMeshData::Material *material = std::get<1>(instance.first);
            Scene::Entity::Skeleton *skeleton = std::get<2>(instance.first);

            {
                auto perObject = mPipeline->mapParameters<ScenePerObject>(2);

                perObject->hasLight = true;

                perObject->hasDistanceField = false;

                perObject->hasTexture = material && material->mDiffuseTexture.available() && material->mDiffuseTexture->mTextureHandle != 0;

                perObject->hasSkeleton = skeleton != nullptr;

                perObject->diffuseColor = material ? material->mDiffuseColor : Vector4::UNIT_SCALE;

                perObject->specularColor = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f };

                perObject->shininess = 32.0f;
            }

            if (skeleton) {
                mPipeline->setDynamicParameters(0, skeleton->matrices());
            } else {
                mPipeline->setDynamicParameters(0, {});
            }

            mPipeline->bindTextures({ { material && material->mDiffuseTexture.available() ? material->mDiffuseTexture->mTextureHandle : 0, TextureType_2D },
                { material && material->mEmissiveTexture.available() ? material->mEmissiveTexture->mTextureHandle : 0, TextureType_2D } });

            std::vector<SceneInstanceData> instanceData;

            std::ranges::transform(instance.second, std::back_inserter(instanceData), [&](const Matrix4 &m) {
                Matrix4 mv = v * m;
                return SceneInstanceData { 
                    mv.Transpose(),
                    mv.Inverse() /*.Transpose().Transpose()*/
                };
            });

            mPipeline->renderMeshInstanced(std::move(instanceData), meshData);
        }
        target->popAnnotation();
    }

    void SceneRenderPass::preRender()
    {
        {
            auto guard = mScene.lock(AccessMode::READ);

            mScene.updateRender();
        }

        mShadowMap->render();
        mPointShadowMaps[0]->render();
        mPointShadowMaps[1]->render();
    }

    int SceneRenderPass::priority() const
    {
        return mPriority;
    }

}
}
