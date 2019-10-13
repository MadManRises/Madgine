#pragma once

#include "Madgine/ui/gamehandler.h"

#include "Madgine/ui/widgetptr.h"

#include "Madgine/scene/camera.h"

namespace ClickBrick {
namespace UI {

    struct GameManager : Engine::UI::GameHandler<GameManager> {

        GameManager(Engine::UI::UIManager &ui);

        virtual bool init() override;

        virtual const char *key() const override;

        virtual void setWidget(Engine::GUI::WidgetBase *w) override;

        virtual void update(std::chrono::microseconds timeSinceLastFrame) override;

        void updateBricks(std::chrono::microseconds timeSinceLastFrame);
        void spawnBrick();

        void onPointerClick(const Engine::Input::PointerEventArgs &evt) override;

        void modScore(int diff);
        void modLife(int diff);

        void start();

    private:
        Engine::Scene::Camera mCamera;

        Engine::UI::WidgetPtr<Engine::GUI::SceneWindow> mGameWindow;
        Engine::UI::WidgetPtr<Engine::GUI::Label> mScoreLabel;
        Engine::UI::WidgetPtr<Engine::GUI::Label> mLifeLabel;

        int mScore = 0;
        int mLife = 100000;

        std::chrono::microseconds mSpawnInterval { 1000000 };
        std::chrono::microseconds mAcc { 0 };

        std::list<Engine::Scene::Entity::Entity *> mBricks;
    };

}
}