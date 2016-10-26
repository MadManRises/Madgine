#pragma once


namespace Maditor {


	namespace View {
		namespace Editors {
			class ScriptEditorTabWidget : public QTabWidget {
				Q_OBJECT

			public:
				ScriptEditorTabWidget(QWidget *parent = 0);

				void setModel(Model::Editors::ScriptEditorModel *model);

			signals:
				void closeDocumentRequest(int index, const QString &path);

			public slots:
				void addDocument(const QString &name, Model::Editors::ScriptEditor *editor);
				void onCloseTabRequest(int index);
				void closeTab(int index);

			};
		}
	}
}