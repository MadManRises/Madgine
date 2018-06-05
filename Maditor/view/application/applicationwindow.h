#pragma once

#include "documents/documentview.h"

#include "componentview.h"

#include "../../model/application/embeddedlauncher.h"
#include "../../model/application/standalonelauncher.h"

namespace Maditor {
namespace View {


	class ApplicationContainerWindow : public QMainWindow, public DocumentView, public ComponentView<Model::ApplicationLauncher> {
		Q_OBJECT

	public:
		ApplicationContainerWindow(Model::ApplicationLauncher *app);

		Model::ApplicationLauncher *app();

	protected:
		void clearModel() override;

		ApplicationLog *logWindow();

	protected slots:
		virtual void onApplicationSettingup();
		virtual void onApplicationSetup();
		virtual void onApplicationShutdown();
		virtual void onApplicationStarted();
		virtual void onApplicationStopped();




	protected:
		QAction *actionSetup;
		QAction *actionShutdown;
		QAction *actionKill;
		QAction *actionStart;
		QAction *actionPause;
		QAction *actionStop;
		



		Model::ApplicationLauncher *mApp;
		ApplicationLog *mLog;

	};

	class StandaloneApplicationContainerWindow : public ApplicationContainerWindow, public ComponentView<Model::StandaloneLauncher>
	{
		Q_OBJECT
	public:
		StandaloneApplicationContainerWindow(Model::StandaloneLauncher *model);

	protected slots:
		void onConnecting();
		void onConnected();
		void onDisconnected();

	private:
		QAction *actionConnect;		
	};

	class EmbeddedApplicationContainerWindow : public ApplicationContainerWindow, public ComponentView<Model::EmbeddedLauncher>
	{
		Q_OBJECT
	public:
		EmbeddedApplicationContainerWindow(Model::EmbeddedLauncher* model);

	protected:
		void clearModel() override;

	protected slots:
		void onApplicationSettingup() override;
		void onApplicationSetup() override;
		void onApplicationShutdown() override;
		void onApplicationStarted() override;
		void onApplicationStopped() override;

		void onProcessError(int errorCode);

		void clicked();

	private:		
		QAction *actionSetup_No_Debug;
		

		ApplicationWindow *mAppWindow;

	};


class ApplicationWindow : public QStackedWidget
{
    Q_OBJECT

public:
	ApplicationWindow(Model::EmbeddedLauncher *app, ApplicationLog *log);

public slots:
	void toggleLog();

signals:
	void clicked();

private:
	Model::ApplicationLauncher *mApp;

};


} // namespace View
} // namespace Maditor

