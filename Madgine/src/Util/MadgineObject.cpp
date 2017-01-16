#pragma once

#include "madginelib.h"
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
				LOG_WARNING(std::string("Double initializeing Object: ") + getName());
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
			mState = ObjectState::CONSTRUCTED;
		}

		
		ObjectState BaseMadgineObject::getState() {
			return mState;
		}

	}
}