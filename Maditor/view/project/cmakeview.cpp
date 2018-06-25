#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "cmakeview.h"




namespace Maditor {
namespace View {

	CmakeView::CmakeView() {

	}

	void CmakeView::setupUi(MainWindow * window)
	{
		ComponentView::setupUi(window);

		mStatusBar = window->statusBar();

		mProgress = new QProgressBar;
		mProgress->setRange(0, 1000);
		mProgress->hide();

		mStatusBar->addPermanentWidget(mProgress);
	}

	void CmakeView::setModel(Model::CmakeServer * server)
	{
		ComponentView::setModel(server);

		ui()->cmakeWidget->setModel(server);

		connect(server, &Model::CmakeServer::statusUpdate, mStatusBar, &QStatusBar::showMessage);
		connect(server, &Model::CmakeServer::processingStarted, mProgress, &QProgressBar::show);
		connect(server, &Model::CmakeServer::processingEnded, mProgress, &QProgressBar::hide);
		connect(server, &Model::CmakeServer::progressUpdate, mProgress, &QProgressBar::setValue);
	}

	void CmakeView::clearModel()
	{
		ui()->cmakeWidget->clearModel();

		ComponentView::clearModel();
	}

} // namespace View
} // namespace Maditor
