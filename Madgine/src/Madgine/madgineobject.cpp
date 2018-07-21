#include "baselib.h"
#include "madgineobject.h"

namespace Engine
{
	MadgineObject::MadgineObject() :
		mState(ObjectState::UNINITIALIZED)
	{
	}

	MadgineObject::~MadgineObject()
	{
		if (mState != ObjectState::UNINITIALIZED)
		{
			LOG_WARNING(std::string("Deleting still initialized Object: ") + mName);
		}
	}


	ObjectState MadgineObject::getState() const
	{
		return mState;
	}
    
    bool MadgineObject::callInit(){
        if (mState == UNINITIALIZED){
            mState = INITIALIZING;
            mName = typeid(*this).name();
            bool result;
            try{
                result = init();
            }catch(const DependencyInitException&){
                result = false;
            }
            if (result)
                mState = INITIALIZED;
            else if (mState == INITIALIZING)
                mState = UNINITIALIZED;
            else if (mState == MARKED_INITIALIZED){
                mState = INITIALIZED;
                callFinalize();
            }else throw 0;
        }
        return mState == INITIALIZED || mState == MARKED_INITIALIZED;
    }
    
    void MadgineObject::callFinalize(){
        if (mState == INITIALIZED){
            finalize();
            mState = UNINITIALIZED;
        }
        assert(mState == UNINITIALIZING);
    }

    bool MadgineObject::init(){
        return true;
    }

    void MadgineObject::finalize(){
    
    }

    void MadgineObject::checkInitState(){
        if (mState == UNINITIALIZED)
            throw DependencyInitException;
    }
    
    void MadgineObject::checkDependency(){
        switch (mState){
        case UNINITIALIZED:
            throw DependencyInitException;
        case INITIALIZING:
            LOG_WARNING("Possible circular dependency! Consider using markInitialized!");
            break;
        default:
        }
    }
    
    void MadgineObject::markInitialized(){
        assert(mState == INITIALIZING);
        mState = MARKED_INITIALIZED;
    }
    
}
