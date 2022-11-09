#pragma once

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"

#include "scenerenderpass.h"

#include "Madgine/render/camera.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT SceneMainWindowComponent : Window::MainWindowComponent<SceneMainWindowComponent> {

        SERIALIZABLEUNIT(SceneMainWindowComponent)

        SceneMainWindowComponent(Window::MainWindow &window);
        ~SceneMainWindowComponent();

        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;

        virtual std::string_view key() const override;

        Scene::SceneManager &scene();

        std::vector<TextureDescriptor> depthTextures();

        Render::RenderTarget *shadowTarget();
        Render::RenderTarget *pointShadowTarget(size_t index);
        Render::RenderData *data();

        Matrix4 getDirectionShadowViewProjectionMatrix();

        void enableSceneRendering();
        void disableSceneRendering();

        Camera mCamera;

        Vector3 mAmbientLightColor = { 1.0f, 1.0f, 1.0f };
        NormalizedVector3 mAmbientLightDirection = { -0.0f, -1.0f, 1.5f };

    private:
        Scene::SceneManager &mScene;

        SceneRenderPass mPass;

        std::unique_ptr<Render::RenderTarget> mShadowMap;
        std::unique_ptr<Render::RenderTarget> mPointShadowMaps[2];

        std::unique_ptr<SceneRenderData> mSceneData;

        ShadowRenderPass mShadowPass;
        PointShadowRenderPass mPointShadowPasses[2];
    };

}
}

REGISTER_TYPE(Engine::Render::SceneMainWindowComponent)