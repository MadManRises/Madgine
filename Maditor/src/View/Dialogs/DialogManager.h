#pragma once


#include "Model\Project\Generator\ClassGeneratorFactory.h"

namespace Maditor {

	namespace View {
		namespace Dialogs {

			class MADITOR_EXPORT DialogManager : public QObject
			{
				Q_OBJECT

			public:
				DialogManager();

				virtual bool confirmFileOverwrite(const QString &filePath, QMessageBox::StandardButton *result = 0);
				static bool confirmFileOverwriteStatic(const QString &filePath, QMessageBox::StandardButton *result = 0);

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
				virtual void onProjectOpened(Model::Project *project);
				virtual void onModuleAdded(Model::Module *module);

				virtual void showNewProjectDialog();
				virtual void showLoadProjectDialog();
				virtual void showNewModuleDialog();
				virtual void showNewClassDialog();

				virtual void showNewGuiHandlerDialog(Model::Module *module, const QString &name);
				virtual void showNewGlobalAPIDialog(Model::Module *module, const QString &name);
				virtual void showNewEntityComponentDialog(Model::Module *module, const QString &name);
				virtual void showNewSceneComponentDialog(Model::Module *module, const QString &name);
				virtual void showNewGameHandlerDialog(Model::Module *module, const QString &name);
				virtual void showNewOtherClassDialog(Model::Module *module, const QString &name);

				virtual void showModulePropertiesDialog(Model::Module *module);

				virtual void showSettingsDialog(Model::Editor *editor);

			private:
				static DialogManager *sSingleton;
			};
		}
	}
}

