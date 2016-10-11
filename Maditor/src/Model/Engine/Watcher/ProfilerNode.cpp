#include "libinclude.h"

#include "ProfilerNode.h"

#include <qlocale.h>

#include "Util\Profiler.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			ProfilerNode::ProfilerNode()
			{
			}
			ProfilerNode::ProfilerNode(const std::string & name, ProfilerNode * parent) :
				TreeItem(parent),
				mName(name),
				mStats(Engine::Util::Profiler::getSingleton().getStats(name)),
				mDuration(0)
			{
				parent->mChildren.push_back(this);

			}
			QVariant ProfilerNode::data(int col)
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
			QVariant ProfilerNode::header(int col)
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
			void ProfilerNode::update(long long frameTime)
			{


				mDuration = mStats->averageDuration();
				mRelDuration = mStats->hasParent() && mStats->parent()->averageDuration() ? ((10000 * mDuration) / mStats->parent()->averageDuration()) / 100.0f : 0.0f;
				mFullRelDuration = ((10000 * mDuration) / frameTime) / 100.0f;

			}
			const std::string & ProfilerNode::name()
			{
				return mName;
			}
			const std::list<ProfilerNode*>& ProfilerNode::children()
			{
				return mChildren;
			}
			const Engine::Util::ProcessStats * ProfilerNode::stats()
			{
				return mStats;
			}
		}
	}
}