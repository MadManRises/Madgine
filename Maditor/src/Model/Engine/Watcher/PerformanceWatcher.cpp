#include "PerformanceWatcher.h"
#include <qdebug.h>
#include "ProfilerNode.h"

#include "libinclude.h"
#include "Util\Profiler.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			PerformanceWatcher::PerformanceWatcher() :
			mUpdatePending(false) {
				startTimer(3000);

				mRootItem = new ProfilerNode;

				connect(this, &PerformanceWatcher::dataChangedQueued, this, &PerformanceWatcher::updateData, Qt::QueuedConnection);
			}

			void PerformanceWatcher::timerEvent(QTimerEvent * event)
			{
				mUpdatePending = true;
			}

			QVariant PerformanceWatcher::headerData(int section, Qt::Orientation orientation, int role) const
			{
				if (role != Qt::DisplayRole)
					return QVariant();

				if (orientation == Qt::Horizontal)
					return ProfilerNode::header(section);
				else
					return QVariant();
			}

			void PerformanceWatcher::update()
			{
				if (mUpdatePending) {
					mUpdatePending = false;

					std::queue<std::pair<ProfilerNode*, QModelIndex>> nodes;

					const std::list<std::string> &topLevelItems = Engine::Util::Profiler::getSingleton().topLevelProcesses();

					if (mRootItem->childCount() < topLevelItems.size()) {
						beginInsertRows(QModelIndex(), mRootItem->childCount(), topLevelItems.size() - 1);
						
						for (const std::string &name : topLevelItems) {
							if (std::find_if(mRootItem->children().begin(), mRootItem->children().end(), [&](ProfilerNode* p) {return p->name() == name; }) == mRootItem->children().end()) {
								
								new ProfilerNode(name, mRootItem);
								
							}
						}
						endInsertRows();
					}

					long long fullDuration = 0;
						
					for (ProfilerNode *n : mRootItem->children()) {
						nodes.push({ n, index(n->index(),0)});
						fullDuration += n->stats()->averageDuration();
					}

					if (fullDuration == 0)
						fullDuration = 1;

					for (; !nodes.empty(); nodes.pop()) {
						ProfilerNode *node = nodes.front().first;
						QModelIndex currentIndex = nodes.front().second;

						node->update(fullDuration);

						const std::list<std::string> &children = node->stats()->children();
						if (node->childCount() < children.size()) {
							beginInsertRows(currentIndex, node->childCount(), children.size() - 1);
							for (const std::string &name : children) {
								if (std::find_if(node->children().begin(), node->children().end(), [&](ProfilerNode* p) {return p->name() == name; }) == node->children().end()) {
									new ProfilerNode(name, node);
								}
							}
							endInsertRows();
						}

						for (ProfilerNode *n : node->children()) {
							nodes.push({ n, index(n->index(), 0, currentIndex) });
						}

					}
					
					emit dataChangedQueued();					
				}
			}

			void PerformanceWatcher::updateData() {
				emit dataChanged(QModelIndex(), QModelIndex());
			}

			Q_INVOKABLE QModelIndex PerformanceWatcher::index(int row, int column, const QModelIndex & parent) const
			{
				if (!hasIndex(row, column, parent))
					return QModelIndex();

				TreeItem *item;
				if (!parent.isValid()) {
					item = mRootItem;
				}
				else {
					item = static_cast<TreeItem*>(parent.internalPointer());
				}


				if (item->childCount() > row) {
					return createIndex(row, column, item->child(row));
				}
				else {
					return QModelIndex();
				}
			}

			Q_INVOKABLE QModelIndex PerformanceWatcher::parent(const QModelIndex & child) const
			{
				if (!child.isValid())
					return QModelIndex();


				TreeItem *item = static_cast<TreeItem*>(child.internalPointer());
				if (item->parentItem() == mRootItem)
					return QModelIndex();

				return createIndex(item->parentItem()->index(), 0, item->parentItem());
			}

			Q_INVOKABLE int PerformanceWatcher::rowCount(const QModelIndex & parent) const
			{
				TreeItem *item;
				if (!parent.isValid())
					item = mRootItem;
				else {
					item = static_cast<TreeItem*>(parent.internalPointer());
				}
				return item->childCount();
			}

			Q_INVOKABLE int PerformanceWatcher::columnCount(const QModelIndex & parent) const
			{
				return 4;
			}

			Q_INVOKABLE QVariant PerformanceWatcher::data(const QModelIndex & index, int role) const
			{
				if (!index.isValid())
					return QVariant();

				if (role != Qt::DisplayRole)
					return QVariant();


				TreeItem *el = static_cast<TreeItem*>(index.internalPointer());
				ProfilerNode *node = static_cast<ProfilerNode*>(el);

				return node->data(index.column());

			}
		}
	}
}