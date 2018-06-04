#pragma once

namespace Maditor {
	namespace Model {
		namespace Editors {

			class ScriptEditorModel : public QObject {
				Q_OBJECT
				
			public:

			signals:
				void documentAdded(const QString &name, ScriptEditor *document);
				void documentClosed(int index);
				void showDoc(ScriptEditor *widget);
				void documentSaved(const QString &name, const QString &group);

			public slots :
				void openScriptFile(const QString &path, int line);
				void closeDocument(int index, const QString &path);
				void onDocumentSaved(ScriptEditor *editor);

			private:
				std::map<QString, ScriptEditor *> mDocuments;

			};
		}
	}
}