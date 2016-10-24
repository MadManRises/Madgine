#include "libinclude.h"
#include "scenelistener.h"

#include "scenemanager.h"

namespace Engine{
namespace Scene{

SceneListener::SceneListener() :
    mSceneMgr(&SceneManager::getSingleton())
{
    mSceneMgr->addSceneListener(this);
}

SceneListener::~SceneListener()
{
    if (SceneManager::getSingletonPtr())
        mSceneMgr->removeSceneListener(this);
}

void SceneListener::notifyEntityRemoved(Entity::Entity *)
{

}

void SceneListener::notifyEntityAdded(Entity::Entity *)
{

}

void SceneListener::onSceneLoad()
{

}

void SceneListener::onSceneClear()
{

}

void SceneListener::beforeSceneClear()
{

}

SceneManager *SceneListener::sceneMgr() const
{
    return mSceneMgr;
}



}
}
