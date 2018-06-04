#pragma once


//#include "Model\Project\Generators\ClassGeneratorFactory.h"

#include "model/dialogmanager.h"

namespace Maditor {

	namespace View {
		namespace Dialogs {

			class DialogManager : public Model::DialogManager
			{

			public:
				DialogManager();
				~DialogManager();

				virtual bool confirmFileOverwrite(const QString &filePath, QMessageBox::StandardButton *result = 0) override;

				virtual SettingsDialog *settingsDialog() override;

				virtual bool showNewProjectDialog(QString &path, QString &name) override;
				virtual bool showLoadProjectDialog(QString &path) override;
				virtual bool showNewConfigDialog(Model::ConfigList *list, QString &name) override;
				virtual bool showNewModuleDialog(Model::ModuleList *list, QString &name) override;
				virtual bool showNewClassDialog(Model::Module *module, QString &name, Model::Generators::ClassGeneratorFactory::ClassType &type) override;

				virtual bool showNewGuiHandlerDialog(Model::Module *module, const QString &name, QString &window, int &type, bool &hasLayout) override;
				virtual bool showNewGlobalAPIDialog(Model::Module *module, const QString &name) override;
				virtual bool showNewEntityComponentDialog(Model::Module *module, const QString &name, QString &componentName) override;
				virtual bool showNewSceneComponentDialog(Model::Module *module, const QString &name) override;
				virtual bool showNewGameHandlerDialog(Model::Module *module, const QString &name) override;
				virtual bool showNewOtherClassDialog(Model::Module *module, const QString &name, bool &headerOnly) override;
				virtual bool showNewServerClassDialog(Model::Module *module, const QString &name) override;

				virtual void showSettingsDialog() override;

				virtual bool showDeleteClassDialog(Model::Generators::ClassGenerator *generator, bool &deleteFiles) override;

			private:
				SettingsDialog *mSettingsDialog;

			};
		}
	}
}

