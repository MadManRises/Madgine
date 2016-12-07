#include "madginelib.h"
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
