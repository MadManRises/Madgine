#pragma once

#include "Madgine/core/frameloop.h"
#include "servertimer.h"

namespace Engine
{
	namespace Server
	{
		
		class MADGINE_SERVER_EXPORT ServerFrameLoop : public Core::FrameLoop
		{
		public:
			virtual int go() override;
			virtual bool singleFrame() override;

		private:
			ServerTimer mTimer;
		};

	}
}
