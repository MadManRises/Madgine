#include "maditorlib.h"

#include "modulepropertiesdialog.h"
#include "ui_modulepropertiesdialog.h"
#include "Model\Project\Module.h"
#include "Model\Project\Project.h"

namespace Maditor {
	namespace View {
		namespace Dialogs {

			ModulePropertiesDialog::ModulePropertiesDialog(Model::Module *module) :
				QDialog(),
				mModule(module),
				ui(new Ui::ModulePropertiesDialog)
			{
				ui->setupUi(this);

				setWindowTitle(windowTitle().arg(module->name()));

				connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ModulePropertiesDialog::apply);

				for (const std::unique_ptr<Model::Module> &dep : *module->parent()) {
					QListWidgetItem *item = new QListWidgetItem(dep->name(), ui->dependenciesList);
					item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
					item->setCheckState(module->dependencies().contains(dep->name()) ? Qt::Checked : Qt::Unchecked);
				}
				
				ui->dependenciesList->sortItems();

			}

			ModulePropertiesDialog::~ModulePropertiesDialog()
			{
				delete ui;
			}

			bool ModulePropertiesDialog::apply()
			{
				bool valid = true;

				for (int i = 0; i < ui->dependenciesList->count(); ++i) {
					QListWidgetItem *item = ui->dependenciesList->item(i);
					bool isChecked = item->checkState() == Qt::Checked;
					bool isDep = mModule->dependencies().contains(item->text());
					if (isChecked != isDep) {
						if (!isChecked) {
							mModule->removeDependency(item->text());
						}
						else {
							if (!mModule->addDependency(item->text())) {
								QMessageBox::critical(0, "Cannot set Dependencies!",
									QString("Dependency to '%1' can not be set to Module '%2' as that would add circular Dependencies!").arg(item->text(), mModule->name()));
								valid = false;
							}
						}
					}
				}

				if (valid) {
					mModule->project()->save();
					mModule->parent()->generate();
				}

				return valid;
			}

		}
	}
}