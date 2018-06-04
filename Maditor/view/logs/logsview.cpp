#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "logsview.h"


namespace Maditor {
namespace View {

	LogsView::LogsView() {

	}

	void LogsView::setupUi(MainWindow * window)
	{
		mLogsWidget = window->ui->logsWidget;
	}

	void LogsView::setModel(Model::LogTableModel * logs)
	{
		ComponentView::setModel(logs);

		mLogsWidget->setModel(model());

	}


} // namespace View
} // namespace Maditor
