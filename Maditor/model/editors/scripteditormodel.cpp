#include "maditorlib.h"

#include "ScriptEditorModel.h"

#include "ScriptEditor.h"


namespace Maditor {
	namespace Model {
		namespace Editors {


			void ScriptEditorModel::openScriptFile(const QString &path, int line) {
				QFileInfo file(path);
				QString name = file.fileName();

				ScriptEditor *editor;
				auto it = mDocuments.find(path);
				if (it == mDocuments.end()) {				
					
					editor = mDocuments.emplace(path, new ScriptEditor(path)).first->second;
					
					connect(editor, &Model::Editors::ScriptEditor::saved, this, &ScriptEditorModel::onDocumentSaved);

					emit documentAdded(name, editor);
				}
				else {
					editor = it->second;
				}
				editor->setFocus();
				editor->setCursorPosition(line-1, 0);
				emit showDoc(editor);
			}

			void ScriptEditorModel::closeDocument(int index, const QString & path)
			{
				auto it = mDocuments.find(path);
				ScriptEditor *editor = it->second;
				emit documentClosed(index);
				mDocuments.erase(it);
			}

			void ScriptEditorModel::onDocumentSaved(ScriptEditor * editor)
			{
				emit documentSaved(editor->getName(), "Scripting");
			}

		}
	}
}