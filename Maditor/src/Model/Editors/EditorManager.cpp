#include "madgineinclude.h"

#include "EditorManager.h"

#include "Addons\Addon.h"

namespace Maditor {
	namespace Model {
		namespace Editors {

			EditorManager *EditorManager::msSingleton = 0;

			EditorManager::EditorManager(Addons::AddonCollector *addons) :
			mAddonCollector(addons){
				msSingleton = this;
			}
			
			/*void EditorManager::openResource(const Ogre::ResourcePtr & res)
			{
				if (res->getOrigin().empty()) return;

				QString type = QString::fromStdString(res->getCreator()->getResourceType());
				int lineNr = -1;

				if (type == "Scripting") {
					Engine::Scripting::Parsing::TextResourcePtr text = res;
					lineNr = text->lineNr();
				}

				open(getFullPath(res->getOrigin(), type), type, lineNr);
			}*/

			QString EditorManager::getFullPath(const std::string & fileName, const QString & group)
			{
				if (!Ogre::ResourceGroupManager::getSingletonPtr()) return QString();
				QString fullPath;
				for (Ogre::ResourceGroupManager::ResourceLocation *loc : Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(group.toStdString())) {
					const Ogre::StringVectorPtr& list = loc->archive->find(fileName);
					if (!list->empty()) {
						fullPath = QString::fromStdString(loc->archive->getName() + "/" + fileName);
						break;
					}
				}
				return fullPath;
			}

			void EditorManager::open(const QString & filePath, const QString &group, int lineNr)
			{
				if (group == "Scripting") {
					mScriptEditor.openScriptFile(filePath, lineNr);
				}
				else {
					if (mAddonCollector) {
						for (Addons::Addon *addon : *mAddonCollector) {
							if (addon->resourceGroupName() == group) {
								addon->openFile(filePath, lineNr);
								return;
							}
						}
					}
					qDebug() << "No Editor available for Resource-Type:" << group;
				}
			}


			void EditorManager::setCurrentRoot(const QString & root)
			{
				mCurrentRoot = root;
			}

			void EditorManager::openByExtension(const std::string & name, int lineNr)
			{
				QString qName = QString::fromStdString(name);

				QFileInfo info(qName);
				if (info.isRelative())
					info.setFile(mCurrentRoot + qName);
				QString suffix = info.suffix();
				QString group;
				if (suffix == "script") {
					group = "Scripting";
				}
				else {
					if (mAddonCollector) {
						for (Addons::Addon *addon : *mAddonCollector) {
							if (addon->supportedFileExtensions().contains(suffix)) {
								group = addon->resourceGroupName();
								break;
							}
						}
					}
					if (group.isEmpty()) {
						qDebug() << "Unknown Extension:" << qName;
						return;
					}
				}
				QString path = info.exists() ? info.filePath() : getFullPath(name, group);
				if (!path.isEmpty())
					open(path, group, lineNr);
				else
					qDebug() << "Could not find Resource-File:" << QString::fromStdString(name);
			}


			ScriptEditorModel * EditorManager::scriptEditor()
			{
				return &mScriptEditor;
			}
			EditorManager & EditorManager::getSingleton()
			{
				assert(msSingleton);
				return *msSingleton;
			}
		}
	}
}