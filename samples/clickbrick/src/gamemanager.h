#pragma once

#include "Madgine/input/handler.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/scenerenderpass.h"

#include "Madgine/render/rendertarget.h"

#include "Generic/intervalclock.h"

#include "Madgine/nativebehaviorcollector.h"

namespace ClickBrick {

    struct GameManager : Engine::Input::Handler<GameManager> {

        GameManager(Engine::Input::UIManager &ui);

        virtual Engine::Threading::Task<bool> init() override;
        virtual Engine::Threading::Task<void> finalize() override;

        virtual std::string_view key() const override;

        virtual void setWidget(Engine::Widgets::WidgetBase *w) override;

        Engine::Threading::Task<void> updateApp();

        void spawnBrick();

        void onPointerClick(const Engine::Input::PointerEventArgs &evt) override;

        void modScore(int diff);
        void modLife(int diff);

        void start();

        Engine::Render::Camera mCamera;

    private:
        Engine::Widgets::SceneWindow *mGameWindow = nullptr;
        Engine::Widgets::Label *mScoreLabel = nullptr;
        Engine::Widgets::Label *mLifeLabel = nullptr;

        int mScore = 0;
        int mLife = 100000;

        Engine::Scene::SceneManager &mSceneMgr;

        std::chrono::microseconds mSpawnInterval;
        std::chrono::microseconds mAcc;
        Engine::IntervalClock<Engine::Threading::CustomTimepoint> mSceneClock;
        
        Engine::Render::SceneRenderPass mSceneRenderer;
        std::unique_ptr<Engine::Render::RenderTarget> mGameRenderTarget;
    };

    Engine::Behavior Brick(float speed, Engine::Vector3 dir, Engine::Quaternion q);
    Engine::Behavior Test();

}

REGISTER_TYPE(ClickBrick::GameManager)