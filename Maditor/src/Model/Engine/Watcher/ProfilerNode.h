#pragma once

#include "Model\TreeItem.h"
#include "forward.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class PerformanceWatcher;
			class StatsProfilerNode;

			class ProfilerNode : public TreeItem {

			public:
				ProfilerNode(const std::string &name);
				virtual ~ProfilerNode();

				// Inherited via TreeItem
				virtual int childCount() override;
				virtual TreeItem * child(int i) override;

			protected:
				ProfilerNode();

				virtual void update(PerformanceWatcher *watcher, const QModelIndex &index, long long fullFrameTime);
			
				
			protected:
				const std::string mName;
				const Engine::Util::ProcessStats * mStats;

			private:
				std::list<StatsProfilerNode *> mChildren;			

			};

			class StatsProfilerNode : public ProfilerNode {
			public:
				StatsProfilerNode(const std::string &name, ProfilerNode *parent);

				virtual void update(PerformanceWatcher *watcher, const QModelIndex &index, long long fullFrameTime) override;

			private:

				ProfilerNode *mParent;

				long long mDuration;
				float mRelDuration, mFullRelDuration;
				

				// Inherited via ProfilerNode
				virtual TreeItem * parentItem() override;

				virtual QVariant data(int col) const override;

			};


			class RootProfilerNode : public ProfilerNode {
			public:
				RootProfilerNode();

				static QVariant header(int col);

				void update(PerformanceWatcher *watcher);
				

				// Inherited via ProfilerNode
				virtual TreeItem * parentItem() override;

				virtual QVariant data(int col) const override;

			};

		}
	}
}