#include "Madgine/clientlib.h"

#include "gamemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/gui/widgets/scenewindow.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/transform.h"

#include "brick.h"

#include "Modules/math/geometry3.h"

#include "Modules/math/boundingbox.h"

#include "Madgine/gui/widgets/label.h"

#include "gameoverhandler.h"

#include "Madgine/app/application.h"

UNIQUECOMPONENT(ClickBrick::UI::GameManager)

RegisterType(ClickBrick::UI::GameManager)

    METATABLE_BEGIN_BASE(ClickBrick::UI::GameManager, Engine::UI::GameHandlerBase)
        METATABLE_END(ClickBrick::UI::GameManager)

            namespace ClickBrick
{
    namespace UI {

        GameManager::GameManager(Engine::UI::UIManager &ui)
            : Engine::UI::GameHandler<GameManager>(ui)
            , mGameWindow(this, "GameView")
            , mScoreLabel(this, "Score")
            , mLifeLabel(this, "Life")
        {
        }

        const char *GameManager::key() const
        {
            return "GameManager";
        }

        bool GameManager::init()
        {

            mCamera.mPosition = { 0, 0, -10 };
            mCamera.mOrientation = {};

            if (mGameWindow) {
                mGameWindow->setCamera(&mCamera);
            }

            return GameHandlerBase::init();
        }

        void GameManager::setWidget(Engine::GUI::WidgetBase *w)
        {
            GameHandlerBase::setWidget(w);

            if (widget()) {
                mGameWindow->setCamera(&mCamera);
            }
        }

        void GameManager::update(std::chrono::microseconds timeSinceLastFrame)
        {
            updateBricks(timeSinceLastFrame);

            mAcc += timeSinceLastFrame;
            while (mAcc > mSpawnInterval) {
                mAcc -= mSpawnInterval;
                mAcc = std::chrono::microseconds { 0 };
                mSpawnInterval *= 999;
                mSpawnInterval /= 1000;
                spawnBrick();
            }
        }

        void ClickBrick::UI::GameManager::updateBricks(std::chrono::microseconds timeSinceLastFrame)
        {
            float ratio = (timeSinceLastFrame.count() / 1000000.0f);

            mBricks.erase(std::remove_if(mBricks.begin(), mBricks.end(), [=](Engine::Scene::Entity::Entity *e) {
                Scene::Brick *brick = e->getComponent<Scene::Brick>();
                Engine::Scene::Entity::Transform *t = e->getComponent<Engine::Scene::Entity::Transform>();
                t->translate(brick->mSpeed * ratio * brick->mDir);

                if (t->getPosition().length() > 10.5f) {
                    e->remove();
                    modLife(-1);
                    return true;
                }

                brick->mQAcc += brick->mQSpeed * 0.1f * ratio;

                if (brick->mQAcc >= 1.0f) {
                    brick->mQAcc = 0.0f;
                    brick->mQ0 = brick->mQ1;

                    Engine::Vector3 orientation = { static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2) };
                    brick->mQ1 = { static_cast<float>(rand()), orientation };
                }

                t->setOrientation(Engine::Slerp(brick->mQ0, brick->mQ1, brick->mQAcc));

                return false;
            }),
                mBricks.end());
        }

        void ClickBrick::UI::GameManager::spawnBrick()
        {
            Engine::Scene::Entity::Entity *brick = Engine::App::Application::getSingleton().getGlobalAPIComponent<Engine::Scene::SceneManager>().createEntity();

            Engine::Scene::Entity::Transform *t = brick->addComponent<Engine::Scene::Entity::Transform>();

            Engine::Vector3 dir = { static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2) };
            dir.normalize();
            t->setPosition(dir * -10);

            Engine::Vector3 orientation = { static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2) };
            Engine::Quaternion q { static_cast<float>(rand()), orientation };
            t->setOrientation(q);

            brick->addComponent<Engine::Scene::Entity::Mesh>()->setName("Brick");

            Scene::Brick *b = brick->addComponent<Scene::Brick>();
            b->mSpeed = rand() / float(RAND_MAX) * 2.0f + 1.0f;
            b->mDir = dir;

            b->mQ1 = q;
            b->mQAcc = 1.0f;
            b->mQSpeed = 1.0f;

            mBricks.push_back(brick);
        }

        void GameManager::onPointerClick(const Engine::Input::PointerEventArgs &evt)
        {
            Engine::Ray ray = mCamera.mousePointToRay(evt.position - mGameWindow->getActualPosition().xy(), mGameWindow->getActualSize().xy());

            LOG(ray.mDir);

            Engine::Scene::Entity::Entity *hit = nullptr;
            float distance = std::numeric_limits<float>::max();

            for (Engine::Scene::Entity::Entity *e : mBricks) {
                const Engine::AABB &aabb = e->getComponent<Engine::Scene::Entity::Mesh>()->aabb();
                Engine::BoundingBox bb = e->getComponent<Engine::Scene::Entity::Transform>()->matrix() * aabb;
                if (Engine::UpTo<float, 2> hits = Engine::Intersect(ray, bb)) {
                    if (hits[0] < distance) {
                        hit = e;
                        distance = hits[0];
                    }
                }
            }

            if (hit) {
                mBricks.remove(hit);
                hit->remove();
                modScore(1);
            }
        }

        void GameManager::modScore(int diff)
        {
            mScore += diff;
            mScoreLabel->mText = "Score:" + std::to_string(mScore);
        }

        void GameManager::modLife(int diff)
        {
            mLife += diff;
            mLifeLabel->mText = "Life: " + std::to_string(mLife);

            if (mLife <= 0) {
                getGuiHandler<GameOverHandler>().setScore(mScore);
                getGuiHandler<GameOverHandler>().open();
            }
        }

        void ClickBrick::UI::GameManager::start()
        {
            mSpawnInterval = std::chrono::microseconds { 1000000 };
            mAcc = std::chrono::microseconds { 0 };

            mScore = 0;
            mScoreLabel->mText = "Score:" + std::to_string(mScore);
            mLife = 3;
            mLifeLabel->mText = "Life: " + std::to_string(mLife);

            for (Engine::Scene::Entity::Entity *brick : mBricks) {
                brick->remove();
            }
            mBricks.clear();
        }

    }
}