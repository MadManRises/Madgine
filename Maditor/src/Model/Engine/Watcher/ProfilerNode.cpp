#include "madgineinclude.h"

#include "ProfilerNode.h"

#include "PerformanceWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			
			ProfilerNode::ProfilerNode(const std::string & name) :
				mName(name),
				mStats(Engine::Util::Profiler::getSingleton().getStats(name))
			{
				for (const std::string &child : mStats->children())
					mChildren.emplace_back(child, this);
			}

			ProfilerNode::~ProfilerNode()
			{
				clear();
			}

			int ProfilerNode::childCount()
			{
				return mChildren.size();
			}

			TreeItem * ProfilerNode::child(int i)
			{
				auto it = mChildren.begin();
				std::advance(it, i);
				return &*it;
			}

			ProfilerNode::ProfilerNode() :
				mStats(0)
			{
			}

			void ProfilerNode::update(PerformanceWatcher * watcher, const QModelIndex & index, long long fullFrameTime)
			{

				const std::list<std::string> &children = mStats->children();
				if (childCount() < children.size()) {
					int oldCount = childCount();

					for (const std::string &name : children) {
						if (std::find_if(mChildren.begin(), mChildren.end(), [&](ProfilerNode& p) {return p.mName == name; }) == mChildren.end()) {
							mChildren.emplace_back(name, this);
						}
					}
					watcher->insertRowsQueued(index, oldCount, childCount() - 1);
				}

				int i = 0;
				for (StatsProfilerNode &n : mChildren) {
					n.update(watcher, watcher->index(i, 0, index), fullFrameTime);
					++i;
				}
				
			}

			void ProfilerNode::clear()
			{
				mChildren.clear();
			}

			StatsProfilerNode::StatsProfilerNode(const std::string & name, ProfilerNode * parent) :
				ProfilerNode(name),
				mParent(parent)
			{

			}

			void StatsProfilerNode::update(PerformanceWatcher *watcher, const QModelIndex &index, long long fullFrameTime)
			{

				mDuration = mStats->averageDuration();
				mRelDuration = mStats->hasParent() && mStats->parent()->averageDuration() ? ((10000 * mDuration) / mStats->parent()->averageDuration()) / 100.0f : 0.0f;
				mFullRelDuration = ((10000 * mDuration) / fullFrameTime) / 100.0f;

				ProfilerNode::update(watcher, index, fullFrameTime);

			}

			QVariant StatsProfilerNode::data(int col) const
			{
				switch (col) {
				case 0:
					return QString::fromStdString(mName);
				case 1:
					return QLocale().toString(mDuration / 1000000.f);
				case 2:
					return mRelDuration;
				case 3:
					return mFullRelDuration;
				default:
					return QVariant();
				}

			}

			TreeItem * StatsProfilerNode::parentItem()
			{
				return mParent;
			}

			RootProfilerNode::RootProfilerNode()
			{
			}

			QVariant RootProfilerNode::header(int col)
			{
				switch (col) {
				case 0:
					return "Name";
				case 1:
					return "Total Time (ms)";
				case 2:
					return "Rel. Time (parent)";
				case 3:
					return "Rel. Time (total)";
				default:
					return QVariant();
				}
			}

			void RootProfilerNode::update(PerformanceWatcher * watcher)
			{
				if (!mStats)
					if (Engine::Util::Profiler::getSingleton().hasStats("Frame"))
						mStats = Engine::Util::Profiler::getSingleton().getStats("Frame");
				if (mStats)
					ProfilerNode::update(watcher, QModelIndex(), std::max((long long)1, mStats->averageDuration()));
			}

			void RootProfilerNode::clear()
			{
				ProfilerNode::clear();
				mStats = 0;
			}

			QVariant RootProfilerNode::data(int col) const
			{
				throw 0;				
			}

			TreeItem * RootProfilerNode::parentItem()
			{
				return 0;
			}

		}
	}
}