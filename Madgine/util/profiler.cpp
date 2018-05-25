#include "../baselib.h"

#include "profiler.h"


namespace Engine
{

	SINGLETON_IMPL(Util::Profiler);

	namespace Util
	{
		Profiler::Profiler() :
			mCurrent(nullptr),
			mInterval(2.0f),
			mCurrentInterval(false)
		{
		}

		void Profiler::startProfiling(const std::string& name)
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
			if (duration >= 1000.f * mInterval)
			{
				mLast = now;
				mCurrentInterval = true;
			}
		}

		ProcessStats& Profiler::getProcess(const std::string& name)
		{
			if (mCurrent)
			{
				return mCurrent->addChild(name);
			}
			return mProcesses.try_emplace(name, [this]() { return mCurrentInterval; }).first->second;
		}

		std::tuple<std::function<bool()>> Profiler::createProcessData()
		{
			return std::make_tuple([this]() { return mCurrentInterval; });
		}


	}
}
