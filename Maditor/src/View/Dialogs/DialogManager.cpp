#include "maditorinclude.h"

#include "DialogManager.h"
#include "Model\Project\ModuleList.h"
#include "Model\Project\Project.h"

#include "newmoduledialog.h"
#include "newclassdialog.h"

#include "newguihandlerdialog.h"

#include "modulepropertiesdialog.h"

#include "loadprojectdialog.h"

#include "settingsdialog.h"


namespace Maditor {
	namespace View {
		namespace Dialogs {

			bool DialogManager::confirmFileOverwrite(const QString &filePath, QMessageBox::StandardButton *result) {
				if (result) {
					if (*result & (QMessageBox::Abort | QMessageBox::NoToAll))
						return false;
					if (*result == QMessageBox::YesToAll)
						return true;
					*result = QMessageBox::question(0, "Overwrite File?", QString("Do you want to overwrite <i>%1</i>?").arg(filePath), QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll | QMessageBox::Abort);
					if (*result & (QMessageBox::No | QMessageBox::NoToAll | QMessageBox::Abort))
						return false;
					else
						return true;
				}
				else {
					QMessageBox::StandardButton val = QMessageBox::question(0, "Overwrite File?", QString("Do you want to overwrite <i>%1</i>?").arg(filePath), QMessageBox::Yes | QMessageBox::No);
					if (val == QMessageBox::No)
						return false;
					else
						return true;
				}
			}

			void DialogManager::onProjectOpened(Model::Project *project) {
				connect(project->moduleList(), &Model::ModuleList::newModuleRequest, this, &DialogManager::showNewModuleDialog);
				connect(this, &DialogManager::newModuleDialogAccepted, project->moduleList(), &Model::ModuleList::createModule);
				connect(project->moduleList(), &Model::ModuleList::moduleAdded, this, &DialogManager::onModuleAdded);

				for (const std::unique_ptr<Model::Module> &module : *project->moduleList()) {
					onModuleAdded(module.get());
				}
			}

			void DialogManager::onModuleAdded(Model::Module * module)
			{
				connect(module, &Model::Module::newClassRequest, this, &DialogManager::showNewClassDialog);
				connect(module, &Model::Module::propertiesDialogRequest, this, &DialogManager::showModulePropertiesDialog);
			}

			void DialogManager::showNewProjectDialog() {
				emit newProjectDialogAccepted("C:/Users/schue/Desktop/GitHub/", "TT");
			}

			void DialogManager::showLoadProjectDialog()
			{
				QString path = QFileDialog::getExistingDirectory(nullptr, "Project Path", QString(), QFileDialog::ShowDirsOnly );
				
				if (!path.isEmpty())
					emit loadProjectDialogAccepted(path + "/");
			}

			void DialogManager::showNewModuleDialog()
			{
				Model::ModuleList *list = static_cast<Model::ModuleList*>(sender());

				NewModuleDialog dialog(list);

				if (dialog.exec() == QDialog::Accepted)
					emit newModuleDialogAccepted(dialog.name());
			}

			void DialogManager::showNewClassDialog()
			{
				Model::Module *module = static_cast<Model::Module*>(sender());

				NewClassDialog dialog(module);

				if (dialog.exec() == QDialog::Accepted)
					emit newClassDialogAccepted(module, dialog.name(), dialog.type());
			}

			void DialogManager::showNewGuiHandlerDialog(Model::Module * module, const QString & name)
			{
				NewGuiHandlerDialog dialog;

				if (dialog.exec() == QDialog::Accepted)
					emit newGuiHandlerDialogAccepted(module, name, dialog.window(), dialog.type(), dialog.layoutFile());
			}

			void DialogManager::showNewGlobalAPIDialog(Model::Module * module, const QString & name)
			{
				emit newGlobalAPIDialogAccepted(module, name);
			}

			void DialogManager::showNewEntityComponentDialog(Model::Module * module, const QString & name)
			{
				emit newEntityComponentDialogAccepted(module, name);
			}

			void DialogManager::showNewSceneComponentDialog(Model::Module * module, const QString & name)
			{
				emit newSceneComponentDialogAccepted(module, name);
			}

			void DialogManager::showNewGameHandlerDialog(Model::Module * module, const QString & name)
			{
				emit newGameHandlerDialogAccepted(module, name);
			}

			void DialogManager::showNewOtherClassDialog(Model::Module * module, const QString & name)
			{
				emit newOtherClassDialogAccepted(module, name);
			}

			void DialogManager::showModulePropertiesDialog(Model::Module * module)
			{
				ModulePropertiesDialog dialog(module);
				
				bool done = false;

				while (!done) {
					done = true;
					if (dialog.exec() == QDialog::Accepted)
						if (!dialog.apply())
							done = false;
				}
			}

			void DialogManager::showSettingsDialog(Model::Editor *editor)
			{
				SettingsDialog dialog(editor);
				dialog.exec();
			}

		}
	}

}

