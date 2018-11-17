#pragma once

#include "processstats.h"


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
					t.start();
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
				ProfilerThread();

				ProcessStats mStats;
			};

			class INTERFACES_EXPORT Profiler
			{
			public:
				Profiler();
				Profiler(const Profiler&) = delete;
				~Profiler();

				const ProcessStats *getThreadStats();

				static Profiler &getSingleton();

			private:
				ProfilerThread mMainThread;		

				static Profiler *sSingleton;
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


#define PROFILE_NAMED(name) thread_local Engine::Debug::Profiler::StaticProcess __sProcess(name); Engine::Debug::Profiler::ProfileGuard<Engine::Debug::Profiler::StaticProcess> __profileGuard{ __sProcess }
#define PROFILE() PROFILE_NAMED(__FUNCTION__)
		}
	}
}
