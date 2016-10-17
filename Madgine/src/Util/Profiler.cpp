#include "libinclude.h"

#include "Profiler.h"


namespace Engine {
	namespace Util {
		
		void Profiler::startProfiling(const std::string &name, const std::string &parent)
		{
			getProcess(name, parent)->start();			
		}

		void Profiler::stopProfiling(const std::string &name)
		{
			getProcess(name)->stop();
		}

		const std::list<std::string>& Profiler::topLevelProcesses()
		{
			return mTopLevelProcesses;
		}

		const ProcessStats * Profiler::getStats(const std::string & name)
		{
			return &mProcesses.find(name)->second;
		}

		ProcessStats * Profiler::getProcess(const std::string & name, const std::string &parent)
		{
			auto it = mProcesses.find(name);
			if (it == mProcesses.end()) {
				
				if (!parent.empty()) {
					mProcesses.find(parent)->second.addChild(name);
					return &mProcesses.emplace(name, &mProcesses[parent]).first->second;
				}
				else {
					mTopLevelProcesses.push_back(name);
					return &mProcesses[name];
				}				
			}
			else {
				return &it->second;
			}
			
		}
		
		ProfileWrapper::ProfileWrapper(const std::string &name, const std::string &parent) :
			mName(name)
		{
			Profiler::getSingleton().startProfiling(name, parent);
		}

		ProfileWrapper::~ProfileWrapper()
		{
			Profiler::getSingleton().stopProfiling(mName);
		}
		
		long long ProcessStats::averageDuration() const
		{
			return mAccumulatedDuration / 20;
		}

		void ProcessStats::start()
		{
			assert(!mStarted);
			mStarted = true;
			mStart = std::chrono::high_resolution_clock::now();
		}

		void ProcessStats::stop()
		{
			assert(mStarted);
			mStarted = false;
			auto end = std::chrono::high_resolution_clock::now();
			long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();

			mAccumulatedDuration -= mBuffer[mRecordIndex];
			mAccumulatedDuration += duration;
			mBuffer[mRecordIndex] = duration;
			++mRecordIndex;
			mRecordIndex %= 20;
		}

		const std::list<std::string>& ProcessStats::children() const
		{
			return mChildren;
		}

		void ProcessStats::addChild(const std::string & child)
		{
			mChildren.push_back(child);
		}

		bool ProcessStats::hasParent() const
		{
			return mParent != 0;
		}

		const ProcessStats * ProcessStats::parent() const
		{
			return mParent;
		}

	}
}