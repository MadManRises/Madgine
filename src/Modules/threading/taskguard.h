#pragma once

#include "task.h"

namespace Engine
{
	namespace Threading
	{
		
		struct TaskGuard
		{
			TaskGuard(TaskHandle &&init, TaskHandle &&finalize) :
				mFinalize(std::move(finalize))
			{
				if (init)
					init();
			}

			~TaskGuard()
			{
				if (mFinalize)
					mFinalize();
			}

		private:
			TaskHandle mFinalize;
		};

	}
}