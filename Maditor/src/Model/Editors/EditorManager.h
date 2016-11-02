#pragma once

#include "VSLink.h"
#include "ScriptEditorModel.h"

namespace Maditor {
	namespace Model {
		namespace Editors {

			class EditorManager : public QObject, public Ogre::Singleton<EditorManager> {
				Q_OBJECT
				
			public:
				EditorManager();

				void openResource(const Ogre::ResourcePtr &res);
				QString getFullPath(const std::string &fileName, const std::string &group);
				void open(const QString &filePath, const std::string &group, int lineNr = -1);
				void openByExtension(const std::string &name, int lineNr = -1);

				VSLink *vs();
				ScriptEditorModel *scriptEditor();

			private:
				VSLink mVS;
				ScriptEditorModel mScriptEditor;
			};
		}
	}
}