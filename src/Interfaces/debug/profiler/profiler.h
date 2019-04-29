#pragma once

#include "processstats.h"

#include "../../threading/threadlocal.h"

namespace Engine
{
	namespace Debug
	{
		namespace Profiler
		{

			template <class T>
			struct ProfileGuard
			{
				ProfileGuard(T &t) : mT(t) 
				{
					mT.start();
				}
				~ProfileGuard()
				{
					mT.stop();
				}
			private:
				T &mT;
			};

			struct INTERFACES_EXPORT ProfilerThread
			{
				ProfilerThread(Profiler *profiler);
				ProfilerThread(ProfilerThread &&) = delete;
				~ProfilerThread();

				std::string mId;
				
				ProcessStats mStats;
				ProcessStats *mCurrent;			
				Profiler *mProfiler;
			};

			class INTERFACES_EXPORT Profiler
			{
			public:
				Profiler();
				Profiler(const Profiler&) = delete;
				~Profiler();

				Profiler &operator=(const Profiler &) = delete;

				const std::list<ProfilerThread> &getThreadStats();

				void registerCurrentThread();

				static Profiler &getCurrent();

			private:
				std::list<ProfilerThread> mThreads;
			};


			struct INTERFACES_EXPORT StaticProcess
			{
				StaticProcess(const char *function) : mStats(function) {}

				void start();
				void stop();
			private:
				ProcessStats mStats;

				ProcessStats *mPrevious = nullptr;

				std::pair<ProcessStats *const, ProcessStats::Data> *mParent = nullptr;
			};


#define PROFILE_NAMED(name) static THREADLOCAL(Engine::Debug::Profiler::StaticProcess) __sProcess(name); Engine::Debug::Profiler::ProfileGuard<Engine::Debug::Profiler::StaticProcess> __profileGuard{ __sProcess }
#define PROFILE() PROFILE_NAMED(__FUNCTION__)
		}
	}
}
