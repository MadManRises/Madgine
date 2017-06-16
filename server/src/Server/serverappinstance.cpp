#include "serverlib.h"
#include "serverappinstance.h"


namespace Engine {
	namespace Server {

		ServerAppInstance::~ServerAppInstance()
		{
			if (mApplication)
				mApplication->shutdown();
			if (mThread.joinable())
				mThread.join();
		}
		

	}
}


