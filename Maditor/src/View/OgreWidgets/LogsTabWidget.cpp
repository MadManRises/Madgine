#include "LogsTabWidget.h"
#include "Model\Engine\Watcher\LogsWatcher.h"
#include <QTableView>
#include <QHeaderView>
#include <qplaintextedit.h>

namespace Maditor {
	namespace View {

		LogsTabWidget::LogsTabWidget(QWidget *parent) :
			QTabWidget(parent){}
		

		void LogsTabWidget::setModel(const Model::Watcher::LogsWatcher * watcher)
		{
			connect(watcher, &Model::Watcher::LogsWatcher::logWatcherCreated, this, &LogsTabWidget::addLogWatcher);
		}

		void LogsTabWidget::addLogWatcher(Model::Watcher::LogWatcher *log) {
			switch (log->type()) {
			case Model::Watcher::LogWatcher::TextLog: {
				QPlainTextEdit *edit = new QPlainTextEdit;
				addTab(edit, QString::fromStdString(log->getName()));
				connect(log, &Model::Watcher::LogWatcher::messageReceived, edit, &QPlainTextEdit::appendPlainText, Qt::QueuedConnection);
				break;
			}
			case Model::Watcher::LogWatcher::GuiLog: {
				QTableView *view = new QTableView;
				view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				view->horizontalHeader()->setStretchLastSection(true);
				view->verticalHeader()->hide();
				view->horizontalHeader()->setMinimumSectionSize(0);
				view->setWordWrap(true);
				view->setModel(log->model());
				//view->setSelectionMode(QAbstractItemView::NoSelection);
				view->setSelectionBehavior(QAbstractItemView::SelectRows);
				addTab(view, QString::fromStdString(log->getName()));

				connect(view, &QTableView::doubleClicked, log->model(), &Model::Watcher::LogTableModel::doubleClicked);
				break;
			}
			}
			
		}

	}

}

