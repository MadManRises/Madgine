#include "baselib.h"
#include "scenecomponent.h"
#include "scenemanager.h"

namespace Engine{

namespace Scene{

	SceneComponentBase::SceneComponentBase(float updateInterval, App::ContextMask context) :
		mContext(context),
		mUpdateInterval(updateInterval),
		mTimeBank(0.f),
		mEnabled(true),
		mSceneMgr(&SceneManager::getSingleton())
{

}

void SceneComponentBase::update(float timeSinceLastFrame, App::ContextMask mask){
    if (mEnabled && (mContext & (mask | App::ContextMask::AnyContext))){
        if (mUpdateInterval == 0.f)
            update(timeSinceLastFrame);
        else {
            mTimeBank += timeSinceLastFrame;
            while (mTimeBank >= mUpdateInterval){                
                mTimeBank -= mUpdateInterval;
                update(mUpdateInterval);
            }
        }
    }
}

bool SceneComponentBase::init()
{
	return MadgineObjectBase::init();
}

void SceneComponentBase::finalize()
{
	MadgineObjectBase::finalize();
}

void SceneComponentBase::setEnabled(bool b)
{
	mEnabled = b;
}

bool SceneComponentBase::isEnabled()
{
	return mEnabled;
}

SceneManager * SceneComponentBase::sceneMgr()
{
	return mSceneMgr;
}

void SceneComponentBase::update(float){}

void SceneComponentBase::setUpdateInterval(float interval)
{
	mUpdateInterval = interval;
}





}
}
