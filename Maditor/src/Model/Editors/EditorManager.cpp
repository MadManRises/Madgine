#include "maditorinclude.h"

#include "EditorManager.h"


namespace Maditor {
	namespace Model {
		namespace Editors {
			EditorManager::EditorManager() {

			}
			
			void EditorManager::openResource(const Ogre::ResourcePtr & res)
			{
				if (res->getOrigin().empty()) return;

				const Ogre::String &type = res->getCreator()->getResourceType();
				int lineNr = -1;

				if (type == "Scripting") {
					Engine::Scripting::Parsing::TextResourcePtr text = res;
					lineNr = text->lineNr();
				}

				open(getFullPath(res->getOrigin(), type), type, lineNr);
			}

			QString EditorManager::getFullPath(const std::string & fileName, const std::string & group)
			{
				if (!Ogre::ResourceGroupManager::getSingletonPtr()) return QString();
				QString fullPath;
				for (Ogre::ResourceGroupManager::ResourceLocation *loc : Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(group)) {
					const Ogre::StringVectorPtr& list = loc->archive->find(fileName);
					if (!list->empty()) {
						fullPath = QString::fromStdString(loc->archive->getName() + "/" + fileName);
						break;
					}
				}
				return fullPath;
			}

			void EditorManager::open(const QString & filePath, const std::string &group, int lineNr)
			{
				if (group == "Scripting") {
					mScriptEditor.openScriptFile(filePath, lineNr);
				}
				else {
					qDebug() << "No Editor available for Resource-Type:" << QString::fromStdString(group);
				}
			}

			void EditorManager::openByExtension(const std::string & name, int lineNr)
			{
				QString qName = QString::fromStdString(name);
				QFileInfo info(qName);
				QString suffix = info.suffix();
				std::string group;
				if (suffix == "script") {
					group = "Scripting";
				}
				else if (QStringList({ "cpp", "h" }).contains(suffix)) {
					qDebug() << qName;
					return;
				}
				else {
					qDebug() << "Unknown Extension:" << qName;
					return;
				}
				QString path = info.exists() ? QString::fromStdString(name) : getFullPath(name, group);
				if (!path.isEmpty())
					open(path, group, lineNr);
				else
					qDebug() << "Could not find Resource-File:" << QString::fromStdString(name);
			}

			VSLink * EditorManager::vs()
			{
				return &mVS;
			}
			ScriptEditorModel * EditorManager::scriptEditor()
			{
				return &mScriptEditor;
			}
		}
	}
}