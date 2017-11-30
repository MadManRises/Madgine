#include "baselib.h"
#include "madgineobject.h"
#include "madgineobjectcollector.h"

namespace Engine
{
	MadgineObject::MadgineObject() :
		mState(ObjectState::CONSTRUCTED)
	{
		MadgineObjectCollector::getSingleton().add(this);
	}

	MadgineObject::~MadgineObject()
	{
		if (mState != ObjectState::CONSTRUCTED)
		{
			LOG_WARNING(std::string("Deleting non-finalized Object: ") + mName + "\n Calling finalize.");
			MadgineObject::finalize();
			if (mState != ObjectState::CONSTRUCTED)
			{
				LOG_WARNING(std::string("Finalize does not call Baseclass-Implementation: ") + mName);
			}
		}
		MadgineObjectCollector::getSingleton().remove(this);
	}

	bool MadgineObject::init()
	{
		if (mState != ObjectState::CONSTRUCTED)
		{
			LOG_WARNING(std::string("Double initializeing Object: ") + mName);
			return false;
		}
		mState = ObjectState::INITIALIZED;
		mName = typeid(*this).name();
		return true;
	}

	void MadgineObject::finalize()
	{
		if (mState == ObjectState::CONSTRUCTED)
		{
			LOG_WARNING(std::string("Finalizing unitialized Object: ") + typeid(*this).name());
		}
		else
		{
			mState = ObjectState::CONSTRUCTED;
		}
	}

	ObjectState MadgineObject::getState() const
	{
		return mState;
	}
}
