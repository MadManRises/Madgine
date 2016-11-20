#include "maditorlib.h"

#include "ProfilerNode.h"

#include "PerformanceWatcher.h"

#include "Common\Shared.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			
			ProfilerNode::ProfilerNode(const std::string & name, const ProcessStats &stats, ProfilerNode *parent) :
				mName(name),
				mStats(stats),
				mParent(parent)
			{
				for (const ProcessStats &child : stats.mChildren) {
					mChildren.emplace_back(child.mName.c_str(), child, this);
				}
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

			void ProfilerNode::update(PerformanceWatcher * watcher, const QModelIndex & index, long long fullFrameTime)
			{
				mDuration = mStats.mAverageDuration;
				mRelDuration = mParent && mParent->mDuration ? ((10000 * mDuration) / mParent->mDuration) / 100.0f : 0.0f;
				mFullRelDuration = ((10000 * mDuration) / fullFrameTime) / 100.0f;

				const SharedList<ProcessStats> &children = mStats.mChildren;
				if (childCount() < children.size()) {
					int oldCount = childCount();

					watcher->beginInsertRows(index, oldCount, children.size() - 1);
					for (const ProcessStats &child : children) {
						if (std::find_if(mChildren.begin(), mChildren.end(), [&](ProfilerNode& p) {return p.mName == child.mName.c_str(); }) == mChildren.end()) {
							mChildren.emplace_back(child.mName.c_str(), child, this);
						}
					}
					watcher->endInsertRows();
				}

				int i = 0;
				for (ProfilerNode &n : mChildren) {
					n.update(watcher, watcher->index(i, 0, index), fullFrameTime);
					++i;
				}
				
			}

			void ProfilerNode::clear()
			{
				mChildren.clear();
			}


			QVariant ProfilerNode::data(int col) const
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

			TreeItem * ProfilerNode::parentItem()
			{
				return mParent;
			}

			RootProfilerNode::RootProfilerNode() :
				ProfilerNode("", SharedMemory::getSingleton().mStats.mRootProcess, 0),
				mShared(SharedMemory::getSingleton().mStats)
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
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
				ProfilerNode::update(watcher, QModelIndex(), std::max((long long)1, mStats.mAverageDuration));
			}

		}
	}
}