#pragma once

#include <chrono>

namespace Engine {
	namespace Util {

		class MADGINE_EXPORT ProcessStats {
		public:
			ProcessStats(ProcessStats *parent = 0) :
				mStarted(false),
				mAccumulatedDuration(0),
				mRecordIndex(0),
				mBuffer(),
				mParent(parent) {}

			long long averageDuration() const;
			void start();
			void stop();

			const std::list<std::string> &children() const;
			void addChild(const std::string &child);

			bool hasParent() const;
			const ProcessStats *parent() const;

		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> mStart;

			bool mStarted;

			long long mAccumulatedDuration;
			size_t mRecordIndex;
			long long mBuffer[20];

			std::list<std::string> mChildren;

			ProcessStats *mParent;

		};


		class MADGINE_EXPORT Profiler : public Ogre::Singleton<Profiler>, public Ogre::GeneralAllocatedObject {
		public:
			void startProfiling(const std::string &name, const std::string &parent = "");
			void stopProfiling(const std::string &name);

			const std::list<std::string> &topLevelProcesses();
			
			bool hasStats(const std::string &name);
			const ProcessStats *getStats(const std::string &name);

		private:
			ProcessStats *getProcess(const std::string &name, const std::string &parent = "");

			std::map<std::string, ProcessStats> mProcesses;
			std::list<std::string> mTopLevelProcesses;
			
		};

		class MADGINE_EXPORT ProfileWrapper {
		public:
			ProfileWrapper(const std::string &name, const std::string &parent = "");
			~ProfileWrapper();

		private:
			std::string mName;
		};

#define PROFILE(TARGET, PARENT) Engine::Util::ProfileWrapper __p(TARGET, PARENT)

	}
}