#include "../moduleslib.h"
#include "connection.h"

namespace Engine
{
	namespace Threading
	{
		ConnectionBase::ConnectionBase(std::shared_ptr<ConnectionBase> *prev) :
			mNext(*prev),
			mPrev(prev)
		{
			if (mNext)
				mNext->mPrev = &mNext;
		}


		void ConnectionBase::disconnect()
		{
			if (mNext)
				mNext->mPrev = mPrev;
			mPrev->swap(mNext);
			mNext.reset();
		}


	}
}
