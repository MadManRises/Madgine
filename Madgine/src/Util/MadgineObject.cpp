#pragma once

#include "libinclude.h"
#include "MadgineObject.h"
#include "MadgineObjectCollector.h"

namespace Engine {
	namespace Util {		

		BaseMadgineObject::BaseMadgineObject() :
			mState(ObjectState::CONSTRUCTED)
		{
			MadgineObjectCollector::getSingleton().add(this);
		}

		BaseMadgineObject::~BaseMadgineObject()
		{
			if (mState != ObjectState::CONSTRUCTED) {
				std::stringstream ss;
				ss << std::hex << (uint64_t)this;
				LOG_WARNING(std::string("Deleting non-finalized Object: ") + ss.str() + "\n Calling finalize.");
				finalize();
				if (mState != ObjectState::CONSTRUCTED) {
					LOG_WARNING(std::string("Finalize does not call Baseclass-Implementation: ") + std::to_string((uint64_t)this));
				}
			}
			MadgineObjectCollector::getSingleton().remove(this);
		}

		void BaseMadgineObject::init()
		{
			if (mState != ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Double initializeingObject: ") + getName());
				return;
			}
			mState = ObjectState::INITIALIZED;
		}

		void BaseMadgineObject::finalize()
		{
			if (mState == ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Finalizing unitialized Object: ") + getName());
				return;
			}
			if (mState == ObjectState::IN_SCENE || mState == ObjectState::IN_SCENE_ABOUT_TO_CLEAR) {
				LOG_WARNING(std::string("Finalizing in-scene Object: ") + getName() + "\n Calling onSceneClear.");
				onSceneClear();
				if (mState != ObjectState::INITIALIZED) {
					LOG_WARNING(std::string("onSceneClear does not call Baseclass-Implementation: ") + getName());
				}
			}
			mState = ObjectState::CONSTRUCTED;
		}

		void BaseMadgineObject::onSceneLoad()
		{
			if (mState == ObjectState::IN_SCENE || mState == ObjectState::IN_SCENE_ABOUT_TO_CLEAR) {
				LOG_WARNING(std::string("OnSceneLoad called on in-scene Object: ") + getName());
				return;
			}
			if (mState == ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("OnSceneLoad called on uninitialized Object: ") + getName() + "\n Calling init.");
				init();
				if (mState != ObjectState::INITIALIZED) {
					LOG_WARNING(std::string("Init does not call Baseclass-Implementation: ") + getName());
				}
			}
			mState = ObjectState::IN_SCENE;
		}

		void BaseMadgineObject::onSceneClear()
		{
			if (mState != ObjectState::IN_SCENE_ABOUT_TO_CLEAR && mState != ObjectState::IN_SCENE) {
				LOG_WARNING(std::string("OnSceneClear called on not in-scene Object: ") + getName() + "\nAborting");
				return;
			}
			if (mState == ObjectState::IN_SCENE) {
				LOG_WARNING(std::string("OnSceneClear called without BeforeSceneClear on Object: ") + getName() + "\n Calling beforeSceneClear.");
				beforeSceneClear();
			}
			mState = ObjectState::INITIALIZED;
		}

		void BaseMadgineObject::beforeSceneClear()
		{
			if (mState != ObjectState::IN_SCENE) {
				LOG_WARNING(std::string("OnSceneClear called on not in-scene Object: ") + getName() + "\nAborting");
				return;
			}
			mState = ObjectState::IN_SCENE_ABOUT_TO_CLEAR;
		}

		ObjectState BaseMadgineObject::getState() {
			return mState;
		}

	}
}