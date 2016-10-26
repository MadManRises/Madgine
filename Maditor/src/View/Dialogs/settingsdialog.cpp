#include "maditorinclude.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "Model\Editor.h"


namespace Maditor {
	namespace View {
		namespace Dialogs {

			SettingsDialog::SettingsDialog(Model::Editor *editor) :
				QDialog(),
				mEditor(editor),
				ui(new Ui::SettingsDialog)
			{
				ui->setupUi(this);

				ui->reloadBox->setChecked(editor->reloadProjectProperty());

				connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);

			}

			SettingsDialog::~SettingsDialog()
			{
				delete ui;
			}

			bool SettingsDialog::apply()
			{
				mEditor->setReloadProjectProperty(ui->reloadBox->isChecked());

				return true;
			}

		}
	}
}