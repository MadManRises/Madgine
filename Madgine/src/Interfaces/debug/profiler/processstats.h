#pragma once

#include "../history.h"

namespace Engine
{
	namespace Debug
	{
		namespace Profiler {

			class INTERFACES_EXPORT ProcessStats
			{
			public:
				ProcessStats(const char *function);
				using Data = Record<std::chrono::nanoseconds, size_t>;

				const char *function() const;
				std::chrono::nanoseconds totalTime() const;

				bool start();
				std::optional<Data> stop();

				std::pair<ProcessStats *const, Data> *updateChild(ProcessStats *child, const Data &data);

				const std::map<ProcessStats *, Data> &children() const;

			private:

				const char *mFunction;

				std::chrono::time_point<std::chrono::high_resolution_clock> mStart;

				size_t mStarted = 0;
				size_t mRunning = 0;

				History<Data, 20> mData;

				std::map<ProcessStats *, Data> mChildren;

			};


		}
	}
}