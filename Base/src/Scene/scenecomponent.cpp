#include "baselib.h"
#include "scenecomponent.h"
#include "scenemanager.h"

namespace Engine{

namespace Scene{

	SceneComponentBase::SceneComponentBase(App::ContextMask context) :
		mContext(context),
		mEnabled(true),
		mSceneMgr(&SceneManagerBase::getSingleton())
{

}

void SceneComponentBase::update(float timeSinceLastFrame, App::ContextMask mask)
{
    if (mEnabled && (mContext & (mask | App::ContextMask::AnyContext))){
		update(timeSinceLastFrame);
    }
}

void SceneComponentBase::fixedUpdate(float timeStep, App::ContextMask mask)
{
	if (mEnabled && (mContext & (mask | App::ContextMask::AnyContext))) {
		fixedUpdate(timeStep);
	}
}

bool SceneComponentBase::init()
{
	return MadgineObject::init();
}

void SceneComponentBase::finalize()
{
	MadgineObject::finalize();
}

void SceneComponentBase::setEnabled(bool b)
{
	mEnabled = b;
}

bool SceneComponentBase::isEnabled()
{
	return mEnabled;
}

SceneManagerBase * SceneComponentBase::sceneMgr()
{
	return mSceneMgr;
}

void SceneComponentBase::update(float){}

void SceneComponentBase::fixedUpdate(float){}

}
}
