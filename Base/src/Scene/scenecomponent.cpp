#include "baselib.h"
#include "scenecomponent.h"
#include "scenemanager.h"

namespace Engine{

namespace Scene{

	BaseSceneComponent::BaseSceneComponent(float updateInterval, App::ContextMask context) :
		mContext(context),
		mUpdateInterval(updateInterval),
		mTimeBank(0.f),
		mEnabled(true),
		mSceneMgr(&SceneManager::getSingleton())
{

}

void BaseSceneComponent::update(float timeSinceLastFrame, App::ContextMask mask){
    if (mEnabled && (mContext & (mask | App::ContextMask::AnyContext))){
        if (mUpdateInterval == 0.f)
            update(timeSinceLastFrame);
        else {
            mTimeBank += timeSinceLastFrame;
            if (mTimeBank >= mUpdateInterval){
                float oldTimeBank = mTimeBank;
                mTimeBank = fmodf(mTimeBank, mUpdateInterval);
                update(oldTimeBank - mTimeBank);
            }
        }
    }
}

void BaseSceneComponent::init()
{
}

void BaseSceneComponent::finalize()
{
}

void BaseSceneComponent::setEnabled(bool b)
{
	mEnabled = b;
}

bool BaseSceneComponent::isEnabled()
{
	return mEnabled;
}

SceneManager * BaseSceneComponent::sceneMgr()
{
	return mSceneMgr;
}

void BaseSceneComponent::update(float){}

void BaseSceneComponent::setUpdateInterval(float interval)
{
	mUpdateInterval = interval;
}





}
}
