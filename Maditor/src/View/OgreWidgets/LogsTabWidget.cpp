#include "maditorinclude.h"

#include "LogsTabWidget.h"
#include "Model\LogsModel.h"
#include "Model\Engine\Watcher\LogWatcher.h"

namespace Maditor {
	namespace View {

		LogsTabWidget::LogsTabWidget(QWidget *parent) :
			QTabWidget(parent){}
		

		void LogsTabWidget::setModel(const Model::LogsModel * model)
		{
			connect(model, &Model::LogsModel::logWatcherCreated, this, &LogsTabWidget::addLog);
			connect(model, &Model::LogsModel::logWatcherDeleted, this, &LogsTabWidget::removeLog);
			for (Model::Log *log : model->logs())
				addLog(log);
		}

		void LogsTabWidget::removeLog(Model::Log * log)
		{
			auto it = mTabs.find(log);
			QWidget *w = it.value();
			removeTab(indexOf(w));
			w->deleteLater();
			mTabs.erase(it);
		}

		void LogsTabWidget::addLog(Model::Log *log) {
			Model::Watcher::OgreLogWatcher *ogreLog = dynamic_cast<Model::Watcher::OgreLogWatcher*>(log);
			if (!ogreLog || ogreLog->type() == Model::Watcher::OgreLogWatcher::TextLog) {
				QPlainTextEdit *edit = new QPlainTextEdit;
				mTabs[log] = edit;
				addTab(edit, QString::fromStdString(log->getName()));
				connect(log, &Model::Log::messageReceived, edit, &QPlainTextEdit::appendPlainText, Qt::QueuedConnection);
			}else{
				QTableView *view = new QTableView;
				mTabs[log] = view;
				view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				view->horizontalHeader()->setStretchLastSection(true);
				view->verticalHeader()->hide();
				view->horizontalHeader()->setMinimumSectionSize(0);
				view->setWordWrap(true);
				view->setModel(ogreLog->model());
				//view->setSelectionMode(QAbstractItemView::NoSelection);
				view->setSelectionBehavior(QAbstractItemView::SelectRows);
				addTab(view, QString::fromStdString(log->getName()));

				connect(view, &QTableView::doubleClicked, ogreLog->model(), &Model::Watcher::LogTableModel::doubleClicked);
			}
			
		}

	}

}

