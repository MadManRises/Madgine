#pragma once

#include "objectstate.h"

namespace Engine
{
	class MADGINE_BASE_EXPORT MadgineObject
	{
    
    
    public:
    
        bool callInit();
        void callFinalize();
    
	protected:
		MadgineObject();
		virtual ~MadgineObject();

		virtual bool init();
		virtual void finalize();

		ObjectState getState() const;
        
        void checkInitState();
        void checkDependency();
        void markInitialized();

	private:
		ObjectState mState;
		std::string mName;
	};
}
