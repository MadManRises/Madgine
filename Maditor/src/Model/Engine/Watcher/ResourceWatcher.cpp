#include "madgineinclude.h"

#include "ResourceWatcher.h"

#include "ResourceGroupItem.h"
#include "ResourceItem.h"

#include "Scripting\Parsing\scriptparser.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			ResourceWatcher::ResourceWatcher() :
				TreeModel(this, 1, true)
			{
				connect(this, &ResourceWatcher::resetModelQueued, this, &ResourceWatcher::resetModel, Qt::QueuedConnection);
			}
			
			ResourceWatcher::~ResourceWatcher()
			{
			}

			void ResourceWatcher::init()
			{
				Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

				int count = 0;
				for (Ogre::ResourceManager *manager : {&Engine::Scripting::Parsing::ScriptParser::getSingleton() }) {
					mGroups.try_emplace(manager->getResourceType().c_str(), this, manager);
					++count;
				}
				insertRowsQueued(QModelIndex(), 0, count - 1);
			}

			
			void ResourceWatcher::clear()
			{
				emit resetModelQueued();
			}

			void ResourceWatcher::resetModel()
			{
				beginResetModel();
				mGroups.clear();
				endResetModel();
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
				return &it->second;
			}

			TreeItem * ResourceWatcher::parentItem()
			{
				throw 0;
			}

			QVariant ResourceWatcher::data(int col) const
			{
				throw 0;
			}

			void ResourceWatcher::reloadScriptFile(const QString & fileName, const QString & group)
			{
				Engine::Scripting::Parsing::ScriptParser::getSingleton().reparseFile(fileName.toStdString(), group.toStdString());
			}

		}
	}
}