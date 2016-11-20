#include "madgineinclude.h"

#include "PerformanceWatcher.h"

#include "Shared.h"



PerformanceWatcher::PerformanceWatcher() :
	mShared(SharedMemory::getSingleton().mStats)
{
}

void PerformanceWatcher::update()
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);

	std::queue<std::pair<SharedList<ProcessStats>*, const std::list<std::string>*>> queue;
	mShared.mRootProcess.mAverageDuration = 0;
	for (const std::string &name : Engine::Util::Profiler::getSingleton().topLevelProcesses()) {
		mShared.mRootProcess.mAverageDuration += Engine::Util::Profiler::getSingleton().getStats(name)->averageDuration();
	}
	queue.emplace(&mShared.mRootProcess.mChildren, &Engine::Util::Profiler::getSingleton().topLevelProcesses());

	while (!queue.empty()) {
		const std::pair<SharedList<ProcessStats>*, const std::list<std::string>*> &p = queue.front();
		auto it = p.second->begin();
		std::advance(it, p.first->size());
		while (it != p.second->end()) {
			ProcessStats &stats = p.first->push();
			stats.mName = it->c_str();
			++it;
		}
		for (ProcessStats &stats : *p.first) {
			const Engine::Util::ProcessStats *process = Engine::Util::Profiler::getSingleton().getStats(stats.mName.c_str());
			stats.mAverageDuration = process->averageDuration();
			queue.emplace(&stats.mChildren, &process->children());
		}
		queue.pop();
	}
}

void PerformanceWatcher::clear() {
	mShared.mRootProcess.mChildren.clear();
}