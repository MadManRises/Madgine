#pragma once

#include "Model\TreeItem.h"
#include "forward.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class ProfilerNode : public TreeItem {

			public:
				ProfilerNode();
				ProfilerNode(const std::string &name, ProfilerNode *parent);

				QVariant data(int col);
				static QVariant header(int col);

				void update(long long frameTime);

				const std::string &name();
				const std::list<ProfilerNode *> &children();
				const Engine::Util::ProcessStats *stats();

			private:
				std::list<ProfilerNode *> mChildren;
				std::string mName;
				const Engine::Util::ProcessStats *mStats;

				long long mDuration;
				float mRelDuration, mFullRelDuration;
			};
		}
	}
}