
#include "baselib.h"
#include "MadgineObject.h"
#include "MadgineObjectCollector.h"
#include "Util\UtilMethods.h"

namespace Engine {	

		MadgineObjectBase::MadgineObjectBase() :
			mState(ObjectState::CONSTRUCTED)
		{
			MadgineObjectCollector::getSingleton().add(this);
		}

		MadgineObjectBase::~MadgineObjectBase()
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

		bool MadgineObjectBase::init()
		{
			if (mState != ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Double initializeing Object: ") + getName());
				return false;
			}
			mState = ObjectState::INITIALIZED;
			return true;
		}

		void MadgineObjectBase::finalize()
		{
			if (mState == ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Finalizing unitialized Object: ") + getName());
				return;
			}
			mState = ObjectState::CONSTRUCTED;
		}

		
		ObjectState MadgineObjectBase::getState() {
			return mState;
		}


}