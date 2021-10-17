#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"
#include "meshdata.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Madgine/render/rendercontext.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/scene/entity/components/pointlight.h"

#define SL_SHADER scene
#include INCLUDE_SL_SHADER

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::SceneRenderPass)
MEMBER(mAmbientFactor)
MEMBER(mDiffuseFactor)
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
        mProgram.create("scene", { sizeof(ScenePerApplication), sizeof(ScenePerFrame), sizeof(ScenePerObject) }, sizeof(SceneInstanceData));        

        mShadowMap = target->context()->createRenderTexture({ 2048, 2048 }, { .mCreateDepthBufferView = true, .mType = TextureType_2DMultiSample, .mSamples = 4, .mTextureCount = 0 });
        mPointShadowMaps[0] = target->context()->createRenderTexture({ 2048, 2048 }, { .mType = TextureType_Cube, .mCreateDepthBufferView = true, .mTextureCount = 0 });
        mPointShadowMaps[1] = target->context()->createRenderTexture({ 2048, 2048 }, { .mType = TextureType_Cube, .mCreateDepthBufferView = true, .mTextureCount = 0 });

        mShadowMap->addRenderPass(&mShadowPass);
        mPointShadowMaps[0]->addRenderPass(&mPointShadowPasses[0]);
        mPointShadowMaps[1]->addRenderPass(&mPointShadowPasses[1]);
    }

    void SceneRenderPass::shutdown()
    {
        mShadowMap.reset();
        mPointShadowMaps[0].reset();
        mPointShadowMaps[1].reset();

        mProgram.reset();
    }

    void SceneRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {
        if (!mProgram.available())
            return;
        //TODO Culling

        Threading::DataLock lock { mScene.mutex(), Threading::AccessMode::READ };

        mScene.updateRender();

        std::map<std::tuple<GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> instances;

        for (const auto &[mesh, e] : mScene.entityComponentList<Scene::Entity::Mesh>().data()) {
            if (!mesh.isVisible())
                continue;

            GPUMeshData *meshData = mesh.data();
            if (!meshData)
                continue;

            const GPUMeshData::Material *material = mesh.material();

            Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
            if (!transform)
                continue;

            Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();

            instances[std::tuple<GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *> { meshData, material, skeleton }].push_back(transform->worldMatrix());
        }

        target->pushAnnotation("Scene");

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        {
            auto perApplication = mProgram.mapParameters(0).cast<ScenePerApplication>();

            perApplication->p = mCamera->getProjectionMatrix(aspectRatio);

            perApplication->hasHDR = target->textureCount() > 1;

            perApplication->ambientFactor = mAmbientFactor;
            perApplication->diffuseFactor = mDiffuseFactor;
        }

        {
            auto perFrame = mProgram.mapParameters(1).cast<ScenePerFrame>();

            perFrame->v = mCamera->getViewMatrix();
            perFrame->light.caster.viewProjectionMatrix = mShadowPass.viewProjectionMatrix();

            perFrame->light.caster.shadowSamples = 4;

            perFrame->light.light.color = mScene.mAmbientLightColor;
            perFrame->light.light.dir = mScene.mAmbientLightDirection;

            Scene::Entity::EntityComponentList<Scene::Entity::PointLight> &lights = mScene.entityComponentList<Scene::Entity::PointLight>();
            perFrame->pointLightCount = lights.size();
            if (perFrame->pointLightCount > 2) {
                LOG_WARNING("Too many point lights in scene!");
                perFrame->pointLightCount = 2;
            }

            for (size_t i = 0; i < perFrame->pointLightCount; ++i) {
                float range = lights[i].mRange;
                perFrame->pointLights[i].position = lights.getEntity(i)->getComponent<Scene::Entity::Transform>()->getPosition();
                perFrame->pointLights[i].color = lights[i].mColor;
                perFrame->pointLights[i].constant = 1.0f;
                perFrame->pointLights[i].linearFactor = 4.5f / range;
                perFrame->pointLights[i].quadratic = 75.0f / (range * range);
            }
        }

        target->bindTextures({ mShadowMap->depthTexture(), mPointShadowMaps[0]->depthTexture(), mPointShadowMaps[1]->depthTexture() }, 2);

        for (const std::pair<const std::tuple<GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> &instance : instances) {
            GPUMeshData *meshData = std::get<0>(instance.first);
            const GPUMeshData::Material *material = std::get<1>(instance.first);
            Scene::Entity::Skeleton *skeleton = std::get<2>(instance.first);

            {
                auto perObject = mProgram.mapParameters(2).cast<ScenePerObject>();

                perObject->hasLight = true;

                perObject->hasDistanceField = false;

                perObject->hasTexture = material && material->mDiffuseTexture->mTextureHandle != 0;

                perObject->hasSkeleton = skeleton != nullptr;

                perObject->diffuseColor = material ? material->mDiffuseColor : Vector4::UNIT_SCALE;
            }

            std::vector<SceneInstanceData> instanceData;

            std::transform(instance.second.begin(), instance.second.end(), std::back_inserter(instanceData), [](const Matrix4 &m) {
                return SceneInstanceData {
                    m,
                    m.Inverse().Transpose()
                };
            });

            mProgram.setInstanceData(std::move(instanceData));

            if (skeleton) {
                mProgram.setDynamicParameters(0, skeleton->matrices());
            } else {
                mProgram.setDynamicParameters(0, {});
            }

            target->renderMeshInstanced(instance.second.size(), meshData, mProgram, material);
        }
        target->popAnnotation();
    }

    void SceneRenderPass::preRender()
    {
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
