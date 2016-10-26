#include "maditorinclude.h"

#include "scripteditortabwidget.h"

#include "Model\Editors\ScriptEditorModel.h"
#include "Model\Editors\ScriptEditor.h"

namespace Maditor {
	namespace View {
		namespace Editors {
			ScriptEditorTabWidget::ScriptEditorTabWidget(QWidget * parent) :
				QTabWidget(parent)
			{
				connect(this, &ScriptEditorTabWidget::tabCloseRequested, this, &ScriptEditorTabWidget::onCloseTabRequest);
			}

			void ScriptEditorTabWidget::addDocument(const QString &name, Model::Editors::ScriptEditor *editor) {

				int index = addTab(editor, name);
				editor->setIndex(index);

				setCurrentIndex(index);
				connect(editor, &QsciScintilla::modificationChanged, [=](bool state) {
					setTabText(index, name + (state ? "*" : ""));
				});
				
			}


			void ScriptEditorTabWidget::setModel(Model::Editors::ScriptEditorModel * model)
			{
				connect(model, &Model::Editors::ScriptEditorModel::documentAdded, this, &ScriptEditorTabWidget::addDocument);
				connect(this, &ScriptEditorTabWidget::closeDocumentRequest, model, &Model::Editors::ScriptEditorModel::closeDocument);
				connect(model, &Model::Editors::ScriptEditorModel::documentClosed, this, &ScriptEditorTabWidget::closeTab);
			}

			void ScriptEditorTabWidget::onCloseTabRequest(int index)
			{
				emit closeDocumentRequest(index, widget(index)->objectName());
			}

			void ScriptEditorTabWidget::closeTab(int index)
			{
				removeTab(index);
			}

		}
	}
}