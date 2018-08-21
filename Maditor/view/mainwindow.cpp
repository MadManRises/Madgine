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
		mUi(new Ui::MainWindow),
		mInspector(nullptr)
	{
		mUi->setupUi(this);		

		mInspector = new InspectorWidget(&mInspectorData);

		mUi->dockWidgetContents->layout()->addWidget(QWidget::createWindowContainer(mInspector));

		QMenu *menu = createPopupMenu();
		menu->setTitle("Views");
		menuBar()->addMenu(menu);

		connect(mUi->actionQuit, &QAction::triggered, this, &MainWindow::close);

	}

MainWindow::~MainWindow()
{	
    delete mUi;

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	bool yesToAll = false, noToAll = false;
	for (DocumentTabWidget *tabWidget : mUi->centralwidget->tabWidgets()) { 
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

	const Ui::MainWindow* MainWindow::ui()
	{
		return mUi;
	}

	InspectorWidget* MainWindow::inspector()
	{
		return mInspector;
	}

	/*Dialogs::DialogManager * MainWindow::dialogs()
{
	return mMaditor->dialogs();
}*/



} // namespace View
} // namespace Maditor
