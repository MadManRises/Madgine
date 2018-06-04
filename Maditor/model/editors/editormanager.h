#pragma once

#include "ScriptEditorModel.h"

namespace Maditor {
	namespace Model {
		namespace Editors {

			class EditorManager : public QObject {
				Q_OBJECT
				
			public:
				EditorManager(Addons::AddonCollector *addons);

				//void openResource(const Ogre::ResourcePtr &res);
				QString getFullPath(const std::string &fileName, const QString &group);
				void open(const QString &filePath, const QString &group, int lineNr = -1);
				void setCurrentRoot(const QString & root);
				void openByExtension(const std::string &name, int lineNr = -1);

				ScriptEditorModel *scriptEditor();

				static EditorManager &getSingleton();

			private:
				ScriptEditorModel mScriptEditor;


				QString mCurrentRoot;

				Addons::AddonCollector *mAddonCollector;

				static EditorManager *msSingleton;
			};
		}
	}
}