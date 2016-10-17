#pragma once 

#include <QDialog>



namespace Maditor {

	namespace Model {
		class Editor;
	};

	namespace View {
		namespace Dialogs {

			namespace Ui {
				class SettingsDialog;
			}


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(Model::Editor *editor);
    ~SettingsDialog();

public slots:
	bool apply();

private:
	Model::Editor *mEditor;

    Ui::SettingsDialog *ui;
};


		}
	}
}
