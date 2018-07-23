#include "maditorviewlib.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"



#include "documents/documentview.h"




#include "../model/maditor.h"
#include "../model/addons/addon.h"


#include "application/inspector/inspectorwidget.h"



namespace Maditor {
namespace View {

	MainWindow::MainWindow() :
		ui(new Ui::MainWindow)		
	{
		const_cast<Ui::MainWindow*>(ui)->setupUi(this);

		ui->dockWidgetContents->layout()->addWidget(QWidget::createWindowContainer(new InspectorWidget));

		QMenu *menu = createPopupMenu();
		menu->setTitle("Views");
		menuBar()->addMenu(menu);

		connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

	}

MainWindow::~MainWindow()
{	
    delete ui;

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



} // namespace View
} // namespace Maditor
