#include "madginelib.h"

#include "Profiler.h"


namespace Engine {
	namespace Util {

		Profiler::Profiler(Serialize::TopLevelSerializableUnitBase *topLevel) :
			SerializableUnit(topLevel),
			mCurrent(nullptr),
			mInterval(2.0f),
			mCurrentInterval(false)
		{
		}

		void Profiler::startProfiling(const std::string &name)
		{
			mCurrent = &getProcess(name);
			mCurrent->start();
		}

		void Profiler::stopProfiling()
		{
			assert(mCurrent);
			mCurrent->stop();
			mCurrent = mCurrent->parent();
		}

		void Profiler::update()
		{
			mCurrentInterval = false;

			auto now = std::chrono::high_resolution_clock::now();
			size_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLast).count();
			if (duration >= 1000.f * mInterval) {
				mLast = now;
				mCurrentInterval = true;
			}
		}

		ProcessStats & Profiler::getProcess(const std::string & name)
		{
			if (mCurrent) {
				return mCurrent->addChild(name);
			}
			else {
				return mProcesses.try_emplace(name, topLevel(), [this]() {return mCurrentInterval; }).first->second;
			}			
		}

		std::tuple<std::string, Serialize::TopLevelSerializableUnitBase*, std::function<bool()>> Profiler::createProcessData(const std::string & name)
		{
			return std::make_tuple(name, topLevel(), [this]() {return mCurrentInterval; });
		}
		
		ProfileWrapper::ProfileWrapper(const std::string &name)
		{
			Profiler *p = Profiler::getSingletonPtr();
			if (p)
				p->startProfiling(name);
		}

		ProfileWrapper::~ProfileWrapper()
		{
			Profiler *p = Profiler::getSingletonPtr();
			if (p)
				p->stopProfiling();
		}
		
		size_t ProcessStats::averageDuration() const
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
			size_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();

			mAccumulatedDuration += duration - mBuffer[mRecordIndex];
			mBuffer[mRecordIndex] = duration;
			++mRecordIndex;
			mRecordIndex %= 20;

			
		}

		ProcessStats &ProcessStats::addChild(const std::string & child)
		{
			return mChildren.try_emplace(child, this).first->second;
		}

		bool ProcessStats::hasParent() const
		{
			return mParent != 0;
		}

		const ProcessStats * ProcessStats::parent() const
		{
			return mParent;
		}

		ProcessStats * ProcessStats::parent()
		{
			return mParent;
		}

	}
}