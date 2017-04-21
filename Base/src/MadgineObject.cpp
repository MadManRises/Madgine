
#include "baselib.h"
#include "MadgineObject.h"
#include "MadgineObjectCollector.h"

namespace Engine {	

		MadgineObject::MadgineObject() :
			mState(ObjectState::CONSTRUCTED)
		{
			MadgineObjectCollector::getSingleton().add(this);
		}

		MadgineObject::~MadgineObject()
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

		const char * MadgineObject::getName()
		{
			return typeid(*this).name();
		}

		bool MadgineObject::init()
		{
			if (mState != ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Double initializeing Object: ") + getName());
				return false;
			}
			mState = ObjectState::INITIALIZED;
			return true;
		}

		void MadgineObject::finalize()
		{
			if (mState == ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Finalizing unitialized Object: ") + getName());
				return;
			}
			mState = ObjectState::CONSTRUCTED;
		}

		
		ObjectState MadgineObject::getState() {
			return mState;
		}


}