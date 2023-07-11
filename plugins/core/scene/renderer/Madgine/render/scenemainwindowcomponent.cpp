#include "../scenerendererlib.h"

#include "scenemainwindowcomponent.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendertarget.h"

#include "Meta/math/matrix4.h"

#include "scenerenderdata.h"

UNIQUECOMPONENT(Engine::Render::SceneMainWindowComponent)

METATABLE_BEGIN(Engine::Render::SceneMainWindowComponent)
MEMBER(mAmbientLightColor)
MEMBER(mAmbientLightDirection)
MEMBER(mCamera)
METATABLE_END(Engine::Render::SceneMainWindowComponent)

SERIALIZETABLE_BEGIN(Engine::Render::SceneMainWindowComponent)
SERIALIZETABLE_END(Engine::Render::SceneMainWindowComponent)

namespace Engine {
namespace Render {

    SceneMainWindowComponent::SceneMainWindowComponent(Window::MainWindow &window)
        : VirtualData(window, 5)
        , mScene(Engine::App::Application::getSingletonPtr() ? &Engine::App::Application::getSingleton().getGlobalAPIComponent<Engine::Scene::SceneManager>() : nullptr)
        , mPass(*this, &mCamera, 1)
        , mShadowPass(*this, &mCamera, 50)
        , mPointShadowPasses { { 0, *this, 50 }, { 1, *this, 50 } }
    {
    }

    Engine::Render::SceneMainWindowComponent::~SceneMainWindowComponent() = default;

    void SceneMainWindowComponent::setup(RenderTarget *target)
    {
        if (mScene) {
            mShadowMap = target->context()->createRenderTexture({ 2048, 2048 }, { .mName = "ShadowMap", .mType = TextureType_2DMultiSample, .mCreateDepthBufferView = true, .mSamples = 4, .mTextureCount = 0 });
            mPointShadowMaps[0] = target->context()->createRenderTexture({ 2048, 2048 }, { .mName = "PointShadowMap0", .mType = TextureType_Cube, .mCreateDepthBufferView = true, .mTextureCount = 0 });
            mPointShadowMaps[1] = target->context()->createRenderTexture({ 2048, 2048 }, { .mName = "PointShadowMap1", .mType = TextureType_Cube, .mCreateDepthBufferView = true, .mTextureCount = 0 });

            mSceneData = std::make_unique<SceneRenderData>(*mScene);

            mShadowMap->addRenderPass(&mShadowPass);
            mPointShadowMaps[0]->addRenderPass(&mPointShadowPasses[0]);
            mPointShadowMaps[1]->addRenderPass(&mPointShadowPasses[1]);

            addDependency(mShadowMap.get());
            addDependency(mPointShadowMaps[0].get());
            addDependency(mPointShadowMaps[1].get());

            addDependency(mSceneData.get());
        }
    }

    void SceneMainWindowComponent::shutdown()
    {
        if (mScene) {
            removeDependency(mShadowMap.get());
            removeDependency(mPointShadowMaps[0].get());
            removeDependency(mPointShadowMaps[1].get());

            removeDependency(mSceneData.get());

            mShadowMap.reset();
            mPointShadowMaps[0].reset();
            mPointShadowMaps[1].reset();

            mSceneData.reset();
        }
    }

    std::string_view SceneMainWindowComponent::key() const
    {
        return "Main-Scene";
    }

    Scene::SceneManager *SceneMainWindowComponent::scene()
    {
        return mScene;
    }

    std::vector<TextureDescriptor> SceneMainWindowComponent::depthTextures()
    {
        return { mShadowMap->depthTexture(), mPointShadowMaps[0]->depthTexture(), mPointShadowMaps[1]->depthTexture() };
    }

    Render::RenderTarget *SceneMainWindowComponent::shadowTarget()
    {
        return mShadowMap.get();
    }

    Render::RenderTarget *SceneMainWindowComponent::pointShadowTarget(size_t index)
    {
        return mPointShadowMaps[index].get();
    }

    Render::RenderData *SceneMainWindowComponent::data()
    {
        return mSceneData.get();
    }

    Matrix4 SceneMainWindowComponent::getDirectionShadowViewProjectionMatrix()
    {
        return mShadowPass.viewProjectionMatrix();
    }

    void SceneMainWindowComponent::enableSceneRendering()
    {
        mWindow.getRenderWindow()->addRenderPass(&mPass);
    }

    void Engine::Render::SceneMainWindowComponent::disableSceneRendering()
    {
        mWindow.getRenderWindow()->removeRenderPass(&mPass);
    }

}
}