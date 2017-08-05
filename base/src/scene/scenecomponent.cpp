#include "baselib.h"
#include "scenecomponent.h"
#include "scenemanager.h"

namespace Engine{

	API_IMPL(Scene::SceneComponentBase, MAP_RO(MasterId, masterId), MAP_RO(SlaveId, slaveId));

namespace Scene{

	SceneComponentBase::SceneComponentBase(ContextMask context) :
		SerializableUnitBase(&SceneManagerBase::getSingleton()),
		mContext(context),
		mEnabled(true),
		mSceneMgr(&SceneManagerBase::getSingleton())
{

}

void SceneComponentBase::update(float timeSinceLastFrame, ContextMask mask)
{
    if (mEnabled && (mContext & (mask | ContextMask::AnyContext))){
		update(timeSinceLastFrame);
    }
}

void SceneComponentBase::fixedUpdate(float timeStep, ContextMask mask)
{
	if (mEnabled && (mContext & (mask | ContextMask::AnyContext))) {
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

Scripting::KeyValueMapList SceneComponentBase::maps()
{
	return MadgineObject::maps().merge(Scripting::API<SceneComponentBase>::sAPI);
}

}
}
