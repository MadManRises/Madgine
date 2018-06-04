#pragma once

#include "projectelement.h"
#include "model/documents/documentstore.h"
#include "moduleselection.h"

#include "generators/generator.h"
#include "model/application/applicationlauncher.h"

namespace Maditor {
	namespace Model {

		class MADITOR_MODEL_EXPORT ApplicationConfig : public Document, public ProjectElement, public Generators::Generator {
			Q_OBJECT

		public:
			enum Launcher {
				MADITOR_LAUNCHER = 0,
				CUSTOM_LAUNCHER = 1
			};


		public:
			ApplicationConfig(ConfigList *parent, const QString &name);
			ApplicationConfig(QDomElement data, ConfigList *parent);
			~ApplicationConfig();

			void generateInfo(Shared::ApplicationInfo &appInfo, QWindow *w = nullptr) const;

			bool hasModuleEnabled(Module *module);

			ApplicationLauncher *createInstace(bool remote);

			Launcher launcher() const;
			const QString &customExecutableCmd() const;
			Shared::LauncherType launcherType() const;
			Generators::ServerClassGenerator *server() const;

			QString path() const override;
			void save() override;
			void discardChanges() override;

			ModuleSelection *modules();

			void release();

		public slots:
			void setLauncher(Launcher launcher);
			void setLauncherType(Shared::LauncherType type);
			void setCustomExecutableCmd(const QString &cmd);
			void setServer(Generators::ServerClassGenerator *server);
			void setServerByName(const QString &name);

		protected:
			void doubleClicked() override;

			bool storeData() override;
			void restoreData() override;

			QStringList filePaths() override;
			void write(QTextStream & stream, int index) override;

		private:
			void init();

		signals:
			void documentCreated(ApplicationLauncher *);
			void documentDestroyed(ApplicationLauncher *);

			void launcherChanged(Launcher newValue, Launcher old);
			void launcherTypeChanged(Shared::LauncherType newValue, Shared::LauncherType old);
			void customExecutableCmdChanged(const QString &);
			void serverChanged(Generators::ServerClassGenerator*);

		private slots:
			void onDocumentCreated(Document *doc);
			void onDocumentDestroyed(Document *doc);


		private:
			ConfigList *mParent;
			DocumentStore mDocuments;


			Launcher mLauncher;
			QString mCustomExecutableCmd;

			Shared::LauncherType mLauncherType;
			Generators::ServerClassGenerator *mServer;

			size_t mInstanceCounter;

			ModuleSelection mModules;


		};

	}
}
