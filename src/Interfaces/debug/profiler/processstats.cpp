#include "../../interfaceslib.h"
#include "processstats.h"


namespace Engine
{
	namespace Debug
	{
		namespace Profiler 
		{
			ProcessStats::ProcessStats(const char * function) :
				mFunction(function)
			{
			}

			const char * ProcessStats::function() const
			{
				return mFunction;
			}

			std::chrono::nanoseconds ProcessStats::totalTime() const
			{
				std::chrono::nanoseconds result = std::get<std::chrono::nanoseconds>(mData.data().mTotal.mData);
				if (mRunning > 0) {
					result += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart);
				}
				return result;
			}

			bool ProcessStats::start()
			{
				++mStarted;
				++mRunning;
				if (mRunning == 1) {
					mStart = std::chrono::high_resolution_clock::now();
					return true;
				}
				else {
					return false;
				}
			}

			std::optional<ProcessStats::Data> ProcessStats::stop()
			{
				--mRunning;
				if (mRunning == 0) {
					auto end = std::chrono::high_resolution_clock::now();
					std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart);
					Data d{duration, mStarted};
					mData << d;
					mStarted = 0;
					return d;
				}
				return {};

				
			}

			std::pair<ProcessStats *const, ProcessStats::Data> * ProcessStats::updateChild(ProcessStats * child, const Data & data)
			{
				auto pib = mChildren.try_emplace(child, data);
				if (!pib.second) {
					pib.first->second += data;
				}
				return &*pib.first;
			}

			const std::map<ProcessStats*, ProcessStats::Data>& ProcessStats::children() const
			{
				return mChildren;
			}

		}
	}
}