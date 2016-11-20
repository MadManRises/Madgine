#pragma once

#include "Model\TreeItem.h"

#include "Common\StatsInfo.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class ProfilerNode : public TreeItem {

			public:
				ProfilerNode(const std::string &name, const ProcessStats &stats, ProfilerNode *parent);
				virtual ~ProfilerNode();

				// Inherited via TreeItem
				virtual int childCount() override;
				virtual TreeItem * child(int i) override;

				

			protected:

				virtual void update(PerformanceWatcher *watcher, const QModelIndex &index, long long fullFrameTime);
				virtual void clear();

				// Inherited via ProfilerNode
				virtual TreeItem * parentItem() override;

				virtual QVariant data(int col) const override;
				
			protected:
				const std::string mName;
				const ProcessStats &mStats;

				ProfilerNode *mParent;

				long long mDuration;
				float mRelDuration, mFullRelDuration;

			private:
				std::list<ProfilerNode> mChildren;			

			};



			class RootProfilerNode : public ProfilerNode {
			public:
				RootProfilerNode();

				static QVariant header(int col);

				void update(PerformanceWatcher *watcher);
				
				using ProfilerNode::clear;

			private:
				SharedStats &mShared;

			};

		}
	}
}