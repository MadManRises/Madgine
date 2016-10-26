#include "maditorinclude.h"

#include "ResourceWatcher.h"

#include "ResourceGroupItem.h"
#include "ResourceItem.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {

			ResourceWatcher::ResourceWatcher() :
				TreeModel(this, 1)
			{
				
			}
			
			ResourceWatcher::~ResourceWatcher()
			{
				for (const std::pair<const std::string, ResourceGroupItem*> &p : mGroups) {
					delete p.second;
				}
			}

			void ResourceWatcher::init()
			{
				Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

				const Ogre::StringVector &groups = rgm.getResourceGroups();
				beginInsertRows(QModelIndex(), 0, groups.size());


				for (Ogre::ResourceManager *manager : {&Engine::Scripting::Parsing::ScriptParser::getSingleton() }) {
					ResourceGroupItem *item = new ResourceGroupItem(this, manager); // ->getResourceType().c_str() );
					mGroups[manager->getResourceType().c_str()] = item;
					
					
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
				ResourceItem *item = dynamic_cast<ResourceItem*>(el);
				if (item) {
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

			int ResourceWatcher::childCount()
			{
				return mGroups.size();
			}

			TreeItem * ResourceWatcher::child(int i)
			{
				auto it = mGroups.begin();
				std::advance(it, i);
				return it->second;
			}

			TreeItem * ResourceWatcher::parentItem()
			{
				throw 0;
			}

			QVariant ResourceWatcher::data(int col) const
			{
				throw 0;
			}

		}
	}
}