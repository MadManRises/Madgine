#include "LogWatcher.h"
#include <qapplication.h>
#include "ResourceWatcher.h"


#include "OgreResourceManager.h"

#include <libinclude.h>
#include "Scripting\Parsing\scriptparser.h"
#include <qdebug.h>
#include "Scripting\Parsing\textResource.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			ResourceWatcher::ResourceWatcher() 
			{
				mRootItem = new TreeItem;
			}
			
			ResourceWatcher::~ResourceWatcher()
			{
				for (const std::pair<const std::string, ResourceGroupItem*> &p : mGroups)
					delete p.second;
				for (const std::pair<const std::string, ResourceItem*> &p : mResources)
					delete p.second;
			}

			void ResourceWatcher::setup()
			{
				Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(this);
			}

			void ResourceWatcher::init()
			{
				Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

				const Ogre::StringVector &groups = rgm.getResourceGroups();
				beginInsertRows(QModelIndex(), 0, groups.size());


				for (Ogre::ResourceManager *manager : {&Engine::Scripting::Parsing::ScriptParser::getSingleton() }) {
					ResourceGroupItem *item = new ResourceGroupItem(mRootItem, manager->getResourceType().c_str() );
					mGroups[manager->getResourceType().c_str()] = item;
					for (const std::pair<Ogre::ResourceHandle, Ogre::ResourcePtr> &res : manager->getResourceIterator()) {
						QString name = res.second->getName().c_str();
						QString actualName = name;
						TreeItem *parent = item;
						if (name.contains("::")) {
							parent = mResources[name.mid(0, name.lastIndexOf("::")).toStdString()];
							actualName = name.mid(name.lastIndexOf("::") + 2);
						}
						
						mResources[name.toStdString()] = new ResourceItem(parent, res.second, actualName);
						
					}
					
					/*Ogre::StringVectorPtr resources = rgm.listResourceNames(group);
					for (const Ogre::String &resource : *resources) {
						rgm._getResourceManager("a")->getByName;
						new TreeItem(item);
					}*/
				}
				endInsertRows();
			}

			void ResourceWatcher::itemDoubleClicked(const QModelIndex &index) {
				TreeItem *el = static_cast<TreeItem*>(index.internalPointer());
				if (el->parentItem() != mRootItem) {
					ResourceItem *item = static_cast<ResourceItem*>(el);
					open(item->resource());
				}
			}

			void ResourceWatcher::open(const Ogre::ResourcePtr & res)
			{
				if (res->getOrigin().empty()) return;

				const Ogre::String &type = res->getCreator()->getResourceType();
				int line = 1;

				if (type == "Scripting") {
					line = static_cast<Engine::Scripting::Parsing::TextResource*>(res.get())->lineNr();
				}
				
				for (Ogre::ResourceGroupManager::ResourceLocation *loc : Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(res->getGroup())) {
					const Ogre::StringVectorPtr& list = loc->archive->find(res->getOrigin());
					if (!list->empty()) {
						emit openScriptFile((loc->archive->getName() + "/" + res->getOrigin()).c_str(), line);
						return;
					}
				}
				qDebug() << "Can't find Resource-File: " << res->getOrigin().c_str();
			}

			void ResourceWatcher::resourceGroupScriptingStarted(const Ogre::String & groupName, size_t scriptCount)
			{
				/*beginInsertRows(QModelIndex(), mRootItem->childCount(), mRootItem->childCount());
				mCurrentGroupItem = new ResourceGroupItem(mRootItem, groupName.c_str());
				endInsertRows();*/
			}

			void ResourceWatcher::scriptParseStarted(const Ogre::String & scriptName, bool & skipThisScript)
			{
			}

			void ResourceWatcher::scriptParseEnded(const Ogre::String & scriptName, bool skipped)
			{
			}

			void ResourceWatcher::resourceGroupScriptingEnded(const Ogre::String & groupName)
			{
			}

			void ResourceWatcher::resourceGroupLoadStarted(const Ogre::String & groupName, size_t resourceCount)
			{

			}

			void ResourceWatcher::resourceLoadStarted(const Ogre::ResourcePtr & resource)
			{
				/*beginInsertRows(createIndex(mRootItem->childCount()-1, 0, mCurrentGroupItem), mCurrentGroupItem->childCount(), mCurrentGroupItem->childCount());
				new ResourceItem(mCurrentGroupItem, resource);
				endInsertRows();*/
			}

			void ResourceWatcher::resourceLoadEnded(void)
			{
			}

			void ResourceWatcher::worldGeometryStageStarted(const Ogre::String & description)
			{
			}

			void ResourceWatcher::worldGeometryStageEnded(void)
			{
			}

			void ResourceWatcher::resourceGroupLoadEnded(const Ogre::String & groupName)
			{
			}

			Q_INVOKABLE QModelIndex ResourceWatcher::index(int row, int column, const QModelIndex & parent) const
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

			Q_INVOKABLE QModelIndex ResourceWatcher::parent(const QModelIndex & child) const
			{
				if (!child.isValid())
					return QModelIndex();


				TreeItem *item = static_cast<TreeItem*>(child.internalPointer());
				if (item->parentItem() == mRootItem)
					return QModelIndex();

				return createIndex(item->parentItem()->index(), 0, item->parentItem());
			}

			Q_INVOKABLE int ResourceWatcher::rowCount(const QModelIndex & parent) const
			{
				TreeItem *item;
				if (!parent.isValid())
					item = mRootItem;
				else {
					item = static_cast<TreeItem*>(parent.internalPointer());
				}
				return item->childCount();
			}

			Q_INVOKABLE int ResourceWatcher::columnCount(const QModelIndex & parent) const
			{
				return 1;
			}

			Q_INVOKABLE QVariant ResourceWatcher::data(const QModelIndex & index, int role) const
			{
				if (!index.isValid())
					return QVariant();

				if (role != Qt::DisplayRole)
					return QVariant();

				TreeItem *el = static_cast<TreeItem*>(index.internalPointer());
				if (el->parentItem() == mRootItem) {
					ResourceGroupItem *group = static_cast<ResourceGroupItem*>(el);
					return group->name();
				}
				else {
					ResourceItem *item = static_cast<ResourceItem*>(el);
					return item->name();
				}
				
			}

			QVariant ResourceWatcher::headerData(int section, Qt::Orientation orientation, int role) const
			{
				if (role != Qt::DecorationRole)
					return QVariant();
				if (orientation == Qt::Horizontal) {
					switch (section) {
					case 0:
						return "Name";
					default:
						return QVariant();
					}
				}
				else {
					return QVariant();
				}
			}

		}
	}
}