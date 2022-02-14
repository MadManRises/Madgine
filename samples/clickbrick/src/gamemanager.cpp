#include "clickbricklib.h"

#include "gamemanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/widgets/scenewindow.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/transform.h"

#include "brick.h"

#include "Meta/math/geometry3.h"

#include "Meta/math/boundingbox.h"

#include "Madgine/widgets/label.h"

#include "gameoverhandler.h"

#include "Madgine/app/application.h"

#include "Meta/math/ray.h"

#include "Madgine/scene/entity/entity.h"

#include "Modules/threading/datamutex.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/render/rendertarget.h"

UNIQUECOMPONENT(ClickBrick::UI::GameManager)

METATABLE_BEGIN_BASE(ClickBrick::UI::GameManager, Engine::UI::GameHandlerBase)
MEMBER(mCamera)
METATABLE_END(ClickBrick::UI::GameManager)

SERIALIZETABLE_INHERIT_BEGIN(ClickBrick::UI::GameManager, Engine::UI::GameHandlerBase)
SERIALIZETABLE_END(ClickBrick::UI::GameManager)

namespace ClickBrick {
namespace UI {

    GameManager::GameManager(Engine::UI::UIManager &ui)
        : Engine::UI::GameHandler<GameManager>(ui)
        , mSceneMgr(Engine::App::Application::getSingleton().getGlobalAPIComponent<Engine::Scene::SceneManager>())
        , mSceneRenderer(mSceneMgr, &mCamera, 50)
    {
    }

    std::string_view GameManager::key() const
    {
        return "GameManager";
    }

    bool GameManager::init()
    {

        mCamera.mPosition = { 0, 0, -10 };
        mCamera.mOrientation = {};

        if (mGameWindow) {
            mGameWindow->getRenderTarget()->addRenderPass(&mSceneRenderer);            
        }

        return GameHandlerBase::init();
    }

    void GameManager::finalize() {

        if (mGameWindow) {
            mGameWindow->getRenderTarget()->removeRenderPass(&mSceneRenderer);
        }

        GameHandlerBase::finalize();
    }

    void GameManager::setWidget(Engine::Widgets::WidgetBase *w)
    {
        GameHandlerBase::setWidget(w);

        if (widget()) {
            mGameWindow = widget()->getChildRecursive<Engine::Widgets::SceneWindow>("GameView");
            mGameWindow->getRenderTarget()->addRenderPass(&mSceneRenderer);  

            mScoreLabel = widget()->getChildRecursive<Engine::Widgets::Label>("Score");
            mLifeLabel = widget()->getChildRecursive<Engine::Widgets::Label>("Life");
        } else {
            mGameWindow = nullptr;
            mScoreLabel = nullptr;
            mLifeLabel = nullptr;
        }
    }

    void GameManager::update(std::chrono::microseconds timeSinceLastFrame)
    {
        if (mSceneMgr.isPaused())
            return;

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

    void GameManager::updateBricks(std::chrono::microseconds timeSinceLastFrame)
    {
        Engine::Threading::DataLock lock { mSceneMgr.mutex(), Engine::Threading::AccessMode::WRITE };

        float ratio = (timeSinceLastFrame.count() / 1000000.0f);

        mBricks.remove_if([=](const Engine::Scene::Entity::EntityPtr &e) {
            if (e.isDead())
                return true;
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

            t->setOrientation(Engine::slerp(brick->mQ0, brick->mQ1, brick->mQAcc));

            return false;
        });
    }

    void ClickBrick::UI::GameManager::spawnBrick()
    {
        Engine::Threading::DataLock lock { Engine::App::Application::getSingleton().getGlobalAPIComponent<Engine::Scene::SceneManager>().mutex(), Engine::Threading::AccessMode::WRITE };

        Engine::Scene::Entity::EntityPtr brick = Engine::App::Application::getSingleton().getGlobalAPIComponent<Engine::Scene::SceneManager>().createEntity();

        Engine::Scene::Entity::Transform *t = brick->addComponent<Engine::Scene::Entity::Transform>().get();
        t->setScale({ 0.01f, 0.01f, 0.01f });

        Engine::Vector3 dir = { static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2) };
        dir.normalize();
        t->setPosition(dir * -10);

        Engine::Vector3 orientation = { static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2), static_cast<float>(rand() - RAND_MAX / 2) };
        Engine::Quaternion q { static_cast<float>(rand()), orientation };
        t->setOrientation(q);

        brick->addComponent<Engine::Scene::Entity::Mesh>().get()->setName("Brick");

        Scene::Brick *b = brick->addComponent<Scene::Brick>().get();
        b->mSpeed = rand() / float(RAND_MAX) * 2.0f + 1.0f;
        b->mDir = dir;

        b->mQ0 = q;
        b->mQ1 = q;
        b->mQAcc = 1.0f;
        b->mQSpeed = 1.0f;

        mBricks.push_back(brick);
    }

    void GameManager::onPointerClick(const Engine::Input::PointerEventArgs &evt)
    {
        Engine::Ray ray = mCamera.mousePointToRay(Engine::Vector2 { static_cast<float>(evt.windowPosition.x), static_cast<float>(evt.windowPosition.y) }, mGameWindow->getActualSize().xy());

        Engine::Scene::Entity::EntityPtr hit;
        float distance = std::numeric_limits<float>::max();        

        for (const Engine::Scene::Entity::EntityPtr &e : mBricks) {
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
            Engine::Threading::DataLock lock { Engine::App::Application::getSingleton().getGlobalAPIComponent<Engine::Scene::SceneManager>().mutex(), Engine::Threading::AccessMode::WRITE };

            mBricks.remove(hit);
            hit->remove();
            modScore(1);
        }
    }

    void GameManager::modScore(int diff)
    {
        mScore += diff;
        mScoreLabel->mText = "Score: " + std::to_string(mScore);
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
        mScoreLabel->mText = "Score: " + std::to_string(mScore);
        mLife = 3;
        mLifeLabel->mText = "Life: " + std::to_string(mLife);

        Engine::Threading::DataLock lock { mSceneMgr.mutex(), Engine::Threading::AccessMode::WRITE };

        for (const Engine::Scene::Entity::EntityPtr &brick : mBricks) {
            brick->remove();
        }
        mBricks.clear();
    }

}
}
