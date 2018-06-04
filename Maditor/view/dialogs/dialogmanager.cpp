#include "maditorviewlib.h"

#include "dialogmanager.h"
#include "model/project/modulelist.h"
#include "model/project/project.h"

#include "newmoduledialog.h"
#include "newclassdialog.h"
#include "newprojectdialog.h"

#include "newguihandlerdialog.h"
#include "newotherclassdialog.h"

#include "settingsdialog.h"


namespace Maditor {
	namespace View {
		namespace Dialogs {

			DialogManager::DialogManager() :
			mSettingsDialog(new SettingsDialog){
			}

			DialogManager::~DialogManager()
			{
				delete mSettingsDialog;
			}

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

			SettingsDialog * DialogManager::settingsDialog()
			{
				return mSettingsDialog;
			}
			
			bool DialogManager::showNewProjectDialog(QString &path, QString &name)
			{
				NewProjectDialog dialog;

				int result = dialog.exec();
				if (result == QDialog::Accepted) {
					path = dialog.path();
					name = dialog.name();
					return true;
				}
				return false;

				path = "C:/Users/schue/Desktop/GitHub/";
				name = "TT";
				return true;
			}
			
			bool DialogManager::showLoadProjectDialog(QString &path)
			{
				path = QFileDialog::getExistingDirectory(nullptr, "Project Path", QString(), QFileDialog::ShowDirsOnly );
				
				return !path.isEmpty();
			}

			bool DialogManager::showNewConfigDialog(Model::ConfigList * list, QString & name)
			{
				bool ok;
				QString s = QInputDialog::getText(nullptr, "New Config...", "Name:", QLineEdit::Normal, QString(), &ok);
				if (s.isEmpty())
					return false;
				if (ok)
					name = s;
				return ok;
			}

			bool DialogManager::showNewModuleDialog(Model::ModuleList *list, QString &name)
			{
				NewModuleDialog dialog(list);

				int result = dialog.exec();
				if (result == QDialog::Accepted) {
					name = dialog.name();
					return true;
				}
				return false;
			}

			bool DialogManager::showNewClassDialog(Model::Module *module, QString &name, Model::Generators::ClassGeneratorFactory::ClassType &type)
			{
				NewClassDialog dialog(module);

				int result = dialog.exec();
				if (result == QDialog::Accepted) {
					name = dialog.name();
					type = dialog.type();
					return true;
				}
				else {
					return false;
				}
			}

			bool DialogManager::showNewGuiHandlerDialog(Model::Module * module, const QString & name, QString &window, int &type, bool &hasLayout)
			{
				NewGuiHandlerDialog dialog;

				int result = dialog.exec();
				if (result == QDialog::Accepted) {
					window = dialog.window();
					type = dialog.type();
					hasLayout = dialog.layoutFile();
					return true;
				}
				else {
					return false;
				}
			}

			bool DialogManager::showNewGlobalAPIDialog(Model::Module * module, const QString & name)
			{
				return true;
			}

			bool DialogManager::showNewEntityComponentDialog(Model::Module * module, const QString & name, QString &componentName)
			{
				componentName = name;
				return true;
			}

			bool DialogManager::showNewSceneComponentDialog(Model::Module * module, const QString & name)
			{
				return true;
			}

			bool DialogManager::showNewGameHandlerDialog(Model::Module * module, const QString & name)
			{
				return true;
			}

			bool DialogManager::showNewOtherClassDialog(Model::Module * module, const QString & name, bool &headerOnly)
			{
				NewOtherClassDialog dialog;

				int result = dialog.exec();
				if (result == QDialog::Accepted) {
					headerOnly = dialog.headerOnly();
					return true;
				}
				else {
					return false;
				}

			}

			bool DialogManager::showNewServerClassDialog(Model::Module * module, const QString & name)
			{
				return true;
			}

			void DialogManager::showSettingsDialog()
			{
				mSettingsDialog->open();
			}

			bool DialogManager::showDeleteClassDialog(Model::Generators::ClassGenerator *generator, bool &deleteFiles)
			{
				QMessageBox msgBox(QMessageBox::Icon::Question, "Delete Class?", QString("Do you really want to delete class <i>%1</i>?").arg(generator->name()), QMessageBox::No | QMessageBox::Yes);

				QCheckBox cb("Delete Files");
				cb.setChecked(true);
				msgBox.setCheckBox(&cb);

				msgBox.exec();


				if (msgBox.result() == QMessageBox::Yes) {
					deleteFiles = cb.isChecked();
					return true;
				}
				else {
					return false;
				}
			}

		}
	}

}

