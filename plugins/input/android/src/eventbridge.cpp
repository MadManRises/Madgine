#include "androidinputlib.h"

#include "eventbridge.h"

namespace Engine
{
	namespace Input
	{

		AInputQueue *sQueue = nullptr;

		void setAndroidInputQueue(AInputQueue *queue)
		{
			sQueue = queue;
		}

	}
}