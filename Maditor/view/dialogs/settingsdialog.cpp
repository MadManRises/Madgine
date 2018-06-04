#include "maditorviewlib.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"


#include "settingstab.h"

namespace Maditor {
	namespace View {
		namespace Dialogs {

			SettingsDialog::SettingsDialog() :
				ui(new Ui::SettingsDialog)
			{
				ui->setupUi(this);				

				connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);

			}

			SettingsDialog::~SettingsDialog()
			{
				delete ui;
			}

			

			void SettingsDialog::addSettingsTab(SettingsTab * tab, const QString & title)
			{
				mTabs.push_back(tab);
				ui->tabWidget->addTab(tab, title);
			}

			void SettingsDialog::open()
			{
				for (SettingsTab *tab : mTabs) {
					tab->setup();
				}
				exec();
			}

			bool SettingsDialog::apply()
			{
				
				bool result = true;

				for (SettingsTab *tab : mTabs) {
					result &= tab->apply();
				}

				return result;
			}

		}
	}
}