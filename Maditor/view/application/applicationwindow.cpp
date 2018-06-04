#include "maditorviewlib.h"


#include "applicationwindow.h"

#include "model/application/applicationlauncher.h"

#include "model/application/ogrewindow.h"

#include "applicationlog.h"

#include "documents/centralwidget.h"
#include "documents/documenttabwidget.h"


namespace Maditor
{
	namespace View
	{
		ApplicationContainerWindow::ApplicationContainerWindow(Model::ApplicationLauncher* app) :
			QMainWindow(nullptr),
			DocumentView(app, this),
			ComponentView(app),
			mApp(app),
			mLog(nullptr)
		{
			connect(app, &Model::ApplicationLauncher::applicationSettingup, this,
			        &ApplicationContainerWindow::onApplicationSettingup);
			connect(app, &Model::ApplicationLauncher::applicationSetup, this,
			        &ApplicationContainerWindow::onApplicationSetup);
			connect(app, &Model::ApplicationLauncher::applicationShutdown, this,
			        &ApplicationContainerWindow::onApplicationShutdown);
			connect(app, &Model::ApplicationLauncher::applicationStarted, this,
			        &ApplicationContainerWindow::onApplicationStarted);
			connect(app, &Model::ApplicationLauncher::applicationStopped, this,
			        &ApplicationContainerWindow::onApplicationStopped);


			actionSetup = new QAction();
			actionSetup->setObjectName(QStringLiteral("actionSetup"));
			actionSetup->setEnabled(false);
			actionSetup->setText(QApplication::translate("Maditor::View::MainWindow", "Setup", 0));
			actionShutdown = new QAction();
			actionShutdown->setObjectName(QStringLiteral("actionShutdown"));
			actionShutdown->setEnabled(false);
			actionShutdown->setText(QApplication::translate("Maditor::View::MainWindow", "Shutdown", 0));
			actionKill = new QAction();
			actionKill->setObjectName(QStringLiteral("actionKill"));
			actionKill->setEnabled(false);
			actionKill->setText(QApplication::translate("Maditor::View::MainWindow", "Kill", 0));


			actionStart = new QAction();
			actionStart->setObjectName(QStringLiteral("actionStart"));
			actionStart->setEnabled(false);
			QIcon icon;
			icon.addFile(QStringLiteral(":/icons/icons/start.jpg"), QSize(), QIcon::Normal, QIcon::Off);
			actionStart->setIcon(icon);
			actionPause = new QAction();
			actionPause->setObjectName(QStringLiteral("actionPause"));
			actionPause->setEnabled(false);
			QIcon icon1;
			icon1.addFile(QStringLiteral(":/icons/icons/pause.png"), QSize(), QIcon::Normal, QIcon::Off);
			actionPause->setIcon(icon1);
			actionStop = new QAction();
			actionStop->setObjectName(QStringLiteral("actionStop"));
			actionStop->setEnabled(false);
			QIcon icon2;
			icon2.addFile(QStringLiteral(":/icons/icons/Stop.jpg"), QSize(), QIcon::Normal, QIcon::Off);
			actionStop->setIcon(icon2);


			setConnections({
				{actionSetup, &Model::ApplicationLauncher::setup},
				{actionShutdown, &Model::ApplicationLauncher::shutdown},
				{actionKill, &Model::ApplicationLauncher::kill},
				{actionStart, &Model::ApplicationLauncher::start},
				{actionPause, &Model::ApplicationLauncher::pause},
				{actionStop, &Model::ApplicationLauncher::stop},
			});


			actionSetup->setEnabled(!app->isSetup());
			actionShutdown->setEnabled(app->isSetup() && app->isLauncher());
			actionKill->setEnabled(app->isSetup());
			actionStart->setEnabled(!app->isRunning() && app->isSetup() && app->isClient());
			actionStop->setEnabled(app->isRunning() && app->isClient());
			actionPause->setEnabled(app->isRunning() && app->isClient());


			if (app->isClient())
			{
				createToolbar("Controls", {
					actionStart,
					actionPause,
					actionStop
				});
			}
			createToolbar("Application", {
				actionSetup,
				actionShutdown,
				actionKill,

			});

			mLog = new ApplicationLog(app);

			setCentralWidget(mLog);

			addItemsToWindow(this);
		}


		StandaloneApplicationContainerWindow::StandaloneApplicationContainerWindow(Model::StandaloneLauncher* model) :
			ApplicationContainerWindow(model),
			ComponentView<Model::StandaloneLauncher>(model)
		{

			connect(model, &Model::StandaloneLauncher::connecting, this, &StandaloneApplicationContainerWindow::onConnecting);
			connect(model, &Model::StandaloneLauncher::connected, this, &StandaloneApplicationContainerWindow::onConnected);
			connect(model, &Model::StandaloneLauncher::disconnected, this, &StandaloneApplicationContainerWindow::onDisconnected);
			

			actionConnect = new QAction();
			actionConnect->setObjectName(QStringLiteral("actionConnect"));
			actionConnect->setEnabled(false);
			actionConnect->setText(QApplication::translate("Maditor::View::MainWindow", "Connect", 0));

			actionConnect->setEnabled(!model->isSetup());
			actionSetup->setEnabled(model->isConnected() && !model->isSetup());

			ComponentView<Model::StandaloneLauncher>::setConnections({

				{actionConnect, &Model::StandaloneLauncher::connect}

			});

			ComponentView<Model::StandaloneLauncher>::createToolbar("Remote", {
				actionConnect
			});

			ComponentView<Model::StandaloneLauncher>::addItemsToWindow(this);
		}

		void StandaloneApplicationContainerWindow::onConnecting()
		{
			actionConnect->setEnabled(false);
		}

		void StandaloneApplicationContainerWindow::onConnected()
		{
			actionSetup->setEnabled(true);
		}

		void StandaloneApplicationContainerWindow::onDisconnected()
		{
			actionConnect->setEnabled(true);
		}

		EmbeddedApplicationContainerWindow::EmbeddedApplicationContainerWindow(Model::EmbeddedLauncher* model):
			ApplicationContainerWindow(model),
			ComponentView<Model::EmbeddedLauncher>(model),
			mAppWindow(nullptr)
		{
			actionSetup_No_Debug = new QAction();
			actionSetup_No_Debug->setObjectName(QStringLiteral("actionSetup_No_Debug"));
			actionSetup_No_Debug->setEnabled(false);
			actionSetup_No_Debug->setText(QApplication::translate("Maditor::View::MainWindow", "Setup (No Debug)", 0));


			actionSetup_No_Debug->setEnabled(!model->isLaunched());

			connect(model, &Model::EmbeddedLauncher::processError, this, &EmbeddedApplicationContainerWindow::onProcessError);


			ComponentView<Model::EmbeddedLauncher>::setConnections({

				{actionSetup_No_Debug, &Model::EmbeddedLauncher::setupNoDebug}

			});

			ComponentView<Model::EmbeddedLauncher>::createToolbar("Embedded", {
				actionSetup_No_Debug,

			});


			ComponentView<Model::EmbeddedLauncher>::addItemsToWindow(this);

			if (model->isClient())
			{
				mAppWindow = new ApplicationWindow(model, logWindow());

				setContextMenuPolicy(Qt::ActionsContextMenu);
				QAction* toggleAction = new QAction("Toggle Log");
				addAction(toggleAction);
				connect(toggleAction, &QAction::triggered, mAppWindow, &ApplicationWindow::toggleLog);
				connect(mAppWindow, &ApplicationWindow::clicked, this, &EmbeddedApplicationContainerWindow::clicked);

				setCentralWidget(mAppWindow);
			}
		}


		void EmbeddedApplicationContainerWindow::onApplicationStopped()
		{
			ApplicationContainerWindow::onApplicationStopped();
			if (mAppWindow && mAppWindow->currentIndex() != 2)
				mAppWindow->setCurrentIndex(0);
		}

		void EmbeddedApplicationContainerWindow::onProcessError(int errorCode)
		{
			QMessageBox::critical(this, "Error", QString("Error code launching game: %1").arg(errorCode));
			ApplicationContainerWindow::onApplicationShutdown();
			actionSetup_No_Debug->setEnabled(true);
		}

		void EmbeddedApplicationContainerWindow::clicked()
		{
			CentralWidget::parentTabWidget(this)->focus();
		}

		void EmbeddedApplicationContainerWindow::clearModel()
		{
			actionSetup->setEnabled(false);
			actionSetup_No_Debug->setEnabled(false);
			actionShutdown->setEnabled(false);
			actionKill->setEnabled(false);
		}

		void EmbeddedApplicationContainerWindow::onApplicationSettingup()
		{
			ApplicationContainerWindow::onApplicationSettingup();
			actionSetup_No_Debug->setEnabled(false);
			if (mAppWindow && mAppWindow->currentIndex() != 2)
				mAppWindow->setCurrentIndex(1);
		}

		void EmbeddedApplicationContainerWindow::onApplicationSetup()
		{
			ApplicationContainerWindow::onApplicationSetup();
			if (mAppWindow && mAppWindow->currentIndex() != 2)
				mAppWindow->setCurrentIndex(0);
		}

		void EmbeddedApplicationContainerWindow::onApplicationStarted()
		{
			ApplicationContainerWindow::onApplicationStarted();
			if (mAppWindow && mAppWindow->currentIndex() != 2)
				mAppWindow->setCurrentIndex(1);
		}

		void EmbeddedApplicationContainerWindow::onApplicationShutdown()
		{
			ApplicationContainerWindow::onApplicationShutdown();
			actionSetup_No_Debug->setEnabled(true);
			if (mAppWindow && mAppWindow->currentIndex() != 2)
				mAppWindow->setCurrentIndex(0);
		}

		Model::ApplicationLauncher* ApplicationContainerWindow::app()
		{
			return mApp;
		}

		void ApplicationContainerWindow::clearModel()
		{
			actionStart->setEnabled(false);
			actionStop->setEnabled(false);
			actionPause->setEnabled(false);
		}

		ApplicationLog* ApplicationContainerWindow::logWindow()
		{
			return mLog;
		}

		void ApplicationContainerWindow::onApplicationSettingup()
		{
			actionSetup->setEnabled(false);
			actionKill->setEnabled(true);
		}

		void ApplicationContainerWindow::onApplicationSetup()
		{
			actionStart->setEnabled(model()->isClient());
			actionShutdown->setEnabled(model()->isLauncher());
		}

		void ApplicationContainerWindow::onApplicationStarted()
		{
			actionStart->setEnabled(false);
			actionPause->setEnabled(true);
			actionStop->setEnabled(true);
		}

		void ApplicationContainerWindow::onApplicationStopped()
		{
			actionStart->setEnabled(true);
			actionPause->setEnabled(false);
			actionStop->setEnabled(false);
		}

		void ApplicationContainerWindow::onApplicationShutdown()
		{
			actionSetup->setEnabled(true);
			actionShutdown->setEnabled(false);
			actionKill->setEnabled(false);
			actionStart->setEnabled(false);
			actionStop->setEnabled(false);
			actionPause->setEnabled(false);
		}

		ApplicationWindow::ApplicationWindow(Model::EmbeddedLauncher* app, ApplicationLog* log) :
			mApp(app)
		{
			QPalette palette;
			QBrush brush(QColor(197, 197, 197, 255));
			brush.setStyle(Qt::SolidPattern);
			palette.setBrush(QPalette::Active, QPalette::Base, brush);
			QBrush brush1(QColor(177, 177, 177, 255));
			brush1.setStyle(Qt::SolidPattern);
			palette.setBrush(QPalette::Active, QPalette::Window, brush1);
			palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
			palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
			palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
			palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
			setPalette(palette);
			setAutoFillBackground(true);


			QWidget* notStarted = new QWidget();
			notStarted->setObjectName(QStringLiteral("notStarted"));
			QGridLayout* gridLayout = new QGridLayout(notStarted);
			gridLayout->setObjectName(QStringLiteral("gridLayout"));
			QLabel* notStartedLabel = new QLabel(notStarted);
			notStartedLabel->setText("The Application is not running!");
			notStartedLabel->setObjectName(QStringLiteral("notStartedLabel"));
			notStartedLabel->setAlignment(Qt::AlignCenter);

			gridLayout->addWidget(notStartedLabel, 0, 0, 1, 1);

			addWidget(notStarted);

			addWidget(QWidget::createWindowContainer(app->window()));
			connect(app->window(), &Model::OgreWindow::clicked, this, &ApplicationWindow::clicked);

			addWidget(log);

			setCurrentIndex(0);
		}


		void ApplicationWindow::toggleLog()
		{
			if (currentIndex() == 2)
			{
				setCurrentIndex(mApp->isRunning() ? 1 : 0);
			}
			else
			{
				setCurrentIndex(2);
			}
		}
	} // namespace View
} // namespace Maditor
