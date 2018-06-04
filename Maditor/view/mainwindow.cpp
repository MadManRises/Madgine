#include "maditorviewlib.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "model/maditor.h"

#include "maditorview.h"

#include "documents/documentview.h"


#include "application/applicationview.h"

#include "model/addons/addon.h"


#include "logs/logsview.h"
#include "project/projectview.h"

namespace Maditor {
namespace View {

	MainWindow::MainWindow() :
		ui(new Ui::MainWindow),
		mLogs(new LogsView),
		mProject(new ProjectView),
		mApplication(new ApplicationView)
	{
		const_cast<Ui::MainWindow*>(ui)->setupUi(this);

		QMenu *menu = createPopupMenu();
		menu->setTitle("Views");
		menuBar()->addMenu(menu);

		connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

		setConnections({
			{ ui->actionNewProject, &Model::Maditor::newProject },
			{ ui->actionLoadProject, &Model::Maditor::loadProject }
		});

		addItemsToWindow(this);

		mApplication->setupUi(this);
		mLogs->setupUi(this);
		mProject->setupUi(this);

		//mMaditor->setModel(model);
	}

MainWindow::~MainWindow()
{
	delete mApplication;
    delete ui;
	delete mLogs;
	delete mProject;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	bool yesToAll = false, noToAll = false;
	for (DocumentTabWidget *tabWidget : ui->centralwidget->tabWidgets()) { 
		for (int i = 0; i < tabWidget->count(); ++i) {
			if (DocumentView *view = dynamic_cast<DocumentView*>(tabWidget->widget(i))) {
				if (yesToAll) {
					view->save();
				}
				else if (noToAll) {
					view->discardChanges();
				}
				else {
					switch (view->requestClose(true)) {
					case QMessageBox::NoToAll:
						noToAll = true;
						break;
					case QMessageBox::YesToAll:
						yesToAll = true;
						break;
					case QMessageBox::Abort:
						event->ignore();
						return;
					default:
						break;
					}
				}
			}
		}
	}


	event->accept();
}

/*Dialogs::DialogManager * MainWindow::dialogs()
{
	return mMaditor->dialogs();
}*/

void MainWindow::setModel(Model::Maditor* model)
{
	connect(model, &Model::Maditor::projectOpened, this, &MainWindow::onProjectOpened);

	if (model->project()) {
		onProjectOpened(model->project());
	}

	mLogs->setModel(model->logs());

	ComponentView::setModel(model);

}


void MainWindow::onProjectOpened(Model::Project *project) {
	mApplication->setConfigModel(project->configList());
	mProject->setModel(project);
}


} // namespace View
} // namespace Maditor
