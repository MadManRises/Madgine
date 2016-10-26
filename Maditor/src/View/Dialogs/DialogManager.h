#pragma once


#include "Model\Project\Generator\ClassGeneratorFactory.h"

namespace Maditor {

	namespace View {
		namespace Dialogs {

			class DialogManager : public QObject
			{
				Q_OBJECT

			public:
				static bool confirmFileOverwrite(const QString &filePath, QMessageBox::StandardButton *result = 0);

			signals:
				void newProjectDialogAccepted(const QString &path, const QString &name);
				void loadProjectDialogAccepted(const QString &path);
				void newModuleDialogAccepted(const QString &name);
				void newClassDialogAccepted(Model::Module *module, const QString &name, Model::Generator::ClassGeneratorFactory::ClassType type);

				void newGuiHandlerDialogAccepted(Model::Module *module, const QString &name, const QString &window, int type, bool hasLayoutFile);
				void newGlobalAPIDialogAccepted(Model::Module *module, const QString &name);
				void newEntityComponentDialogAccepted(Model::Module *module, const QString &name);
				void newSceneComponentDialogAccepted(Model::Module *module, const QString &name);
				void newGameHandlerDialogAccepted(Model::Module *module, const QString &name);
				void newOtherClassDialogAccepted(Model::Module *module, const QString &name);

			public slots:
				void onProjectOpened(Model::Project *project);
				void onModuleAdded(Model::Module *module);

				void showNewProjectDialog();
				void showLoadProjectDialog();
				void showNewModuleDialog();
				void showNewClassDialog();

				void showNewGuiHandlerDialog(Model::Module *module, const QString &name);
				void showNewGlobalAPIDialog(Model::Module *module, const QString &name);
				void showNewEntityComponentDialog(Model::Module *module, const QString &name);
				void showNewSceneComponentDialog(Model::Module *module, const QString &name);
				void showNewGameHandlerDialog(Model::Module *module, const QString &name);
				void showNewOtherClassDialog(Model::Module *module, const QString &name);

				void showModulePropertiesDialog(Model::Module *module);

				void showSettingsDialog(Model::Editor *editor);
			};
		}
	}
}

