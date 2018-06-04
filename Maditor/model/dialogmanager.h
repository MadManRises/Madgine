#pragma once


#include "model/project/generators/classgeneratorfactory.h"


namespace Maditor {

	namespace Model {


			class MADITOR_MODEL_EXPORT DialogManager : public QObject
			{
				Q_OBJECT

			public:
				DialogManager() {
					sSingleton = this;
				};
				~DialogManager() = default;

				virtual bool confirmFileOverwrite(const QString &filePath, QMessageBox::StandardButton *result = 0) = 0;
				static bool confirmFileOverwriteStatic(const QString &filePath, QMessageBox::StandardButton *result = 0) {
					return sSingleton->confirmFileOverwrite(filePath, result);
				}

				virtual View::Dialogs::SettingsDialog *settingsDialog() = 0;
					

				virtual bool showNewProjectDialog(QString &path, QString &name) = 0;
				virtual bool showLoadProjectDialog(QString &path) = 0;
				virtual bool showNewConfigDialog(Model::ConfigList *list, QString &name) = 0;
				static bool showNewConfigDialogStatic(Model::ConfigList *list, QString &name) {
					return sSingleton->showNewConfigDialog(list, name);
				}
				virtual bool showNewModuleDialog(Model::ModuleList *list, QString &name) = 0;
				static bool showNewModuleDialogStatic(Model::ModuleList *list, QString &name) {
					return sSingleton->showNewModuleDialog(list, name);
				}
				virtual bool showNewClassDialog(Model::Module *module, QString &name, Model::Generators::ClassGeneratorFactory::ClassType &type) = 0;
				static bool showNewClassDialogStatic(Model::Module *module, QString &name, Model::Generators::ClassGeneratorFactory::ClassType &type) {
					return sSingleton->showNewClassDialog(module, name, type);
				}

				virtual bool showNewGuiHandlerDialog(Model::Module *module, const QString &name, QString &window, int &type, bool &hasLayout) = 0;
				virtual bool showNewGlobalAPIDialog(Model::Module *module, const QString &name) = 0;
				virtual bool showNewEntityComponentDialog(Model::Module *module, const QString &name, QString &componentName) = 0;
				virtual bool showNewSceneComponentDialog(Model::Module *module, const QString &name) = 0;
				virtual bool showNewGameHandlerDialog(Model::Module *module, const QString &name) = 0;
				virtual bool showNewOtherClassDialog(Model::Module *module, const QString &name, bool &headerOnly) = 0;
				virtual bool showNewServerClassDialog(Model::Module *module, const QString &name) = 0;
				static bool showNewGuiHandlerDialogStatic(Model::Module *module, const QString &name, QString &window, int &type, bool &hasLayout) {
					return sSingleton->showNewGuiHandlerDialog(module, name, window, type, hasLayout);
				}
				static bool showNewGlobalAPIDialogStatic(Model::Module *module, const QString &name) {
					return sSingleton->showNewGlobalAPIDialog(module, name);
				}
				static bool showNewEntityComponentDialogStatic(Model::Module *module, const QString &name, QString &componentName) {
					return sSingleton->showNewEntityComponentDialog(module, name, componentName);
				}
				static bool showNewSceneComponentDialogStatic(Model::Module *module, const QString &name) {
					return sSingleton->showNewSceneComponentDialog(module, name);
				}
				static bool showNewGameHandlerDialogStatic(Model::Module *module, const QString &name) {
					return sSingleton->showNewGameHandlerDialog(module, name);
				}
				static bool showNewOtherClassDialogStatic(Model::Module *module, const QString &name, bool &headerOnly) {
					return sSingleton->showNewOtherClassDialog(module, name, headerOnly);
				}
				static bool showNewServerClassDialogStatic(Model::Module *module, const QString &name) {
					return sSingleton->showNewServerClassDialog(module, name);
				}

				virtual void showSettingsDialog() = 0;

				virtual bool showDeleteClassDialog(Generators::ClassGenerator *generator, bool &deleteFiles) = 0;
				static bool showDeleteClassDialogStatic(Generators::ClassGenerator *generator, bool &deleteFiles) {
					return sSingleton->showDeleteClassDialog(generator, deleteFiles);
				}

			private:
				static DialogManager *sSingleton;

				
			};
	}
}

