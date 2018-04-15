#pragma once

#include "../generic/singleton.h"

namespace Engine
{
	namespace SignalSlot
	{
		
		class INTERFACES_EXPORT ConnectionManager : public Singleton<ConnectionManager>
		{
		public:
			ConnectionManager();

			void queue(std::function<void()>&& f);

			void update();

			std::thread::id thread() const;

		private:

			std::thread::id mThread;
			std::queue<std::function<void()>> mQueue;
			std::mutex mMutex;
		};

	}
}