
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
				LOG_WARNING(std::string("Deleting non-finalized Object: ") + mName + "\n Calling finalize.");
				finalize();
				if (mState != ObjectState::CONSTRUCTED) {
					LOG_WARNING(std::string("Finalize does not call Baseclass-Implementation: ") + mName);
				}
			}
			MadgineObjectCollector::getSingleton().remove(this);
		}

		bool MadgineObject::init()
		{
			
			if (mState != ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Double initializeing Object: ") + mName);
				return false;
			}
			if (ScopeBase::init()) {
				mState = ObjectState::INITIALIZED;
				mName = getIdentifier();
				return true;
			}
			else {
				return false;
			}
		}

		void MadgineObject::finalize()
		{
			ScopeBase::finalize();
			if (mState == ObjectState::CONSTRUCTED) {
				LOG_WARNING(std::string("Finalizing unitialized Object: ") + getIdentifier());
			}else{
				mState = ObjectState::CONSTRUCTED;
			}			
		}
		
		ObjectState MadgineObject::getState() {
			return mState;
		}


}