#include "maditorviewlib.h"

#include "logwidget.h"
#include "ui_logwidget.h"
#include "model/logs/logtablemodel.h"

namespace Maditor {
	namespace View {


		LogWidget::LogWidget(QWidget *parent) :
			QWidget(parent),
			ui(new Ui::LogWidget),
			mLastIndex(-1)
		{
			ui->setupUi(this);

			if (ui->tableView->horizontalHeader()->isSortIndicatorShown())
				ui->tableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

			connect(ui->tableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &LogWidget::sortIndicatorChanged);
		}

		LogWidget::~LogWidget()
		{
			delete ui;
		}

		void LogWidget::setModel(Model::LogTableModel * log)
		{
			//connect(ui->tableView, &QTableView::doubleClicked, log, &Model::Watcher::LogTableModel::doubleClicked);
			ui->tableView->setModel(log->sorted());
			connect(ui->clearButton, &QPushButton::clicked, log, &Model::LogTableModel::clear);

			connect(log, &Model::LogTableModel::rowsInserted, this, &LogWidget::resize);
			
			ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
			ui->tableView->setColumnWidth(0, 24);
			ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
			ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
			ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

			ui->tableView->horizontalHeader()->setStretchLastSection(false);
			//ui->tableView->horizontalHeader()->
		}

		void LogWidget::sortIndicatorChanged(int index, Qt::SortOrder order)
		{
			if (mLastIndex == index && order == Qt::AscendingOrder) {
				ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
				mLastIndex = -1;
			}
			else {
				mLastIndex = index;
			}
		}

		void LogWidget::resize() {
			/*ui->tableView->resizeColumnToContents(3);
			ui->tableView->resizeColumnToContents(2);
			ui->tableView->resizeRowsToContents();*/ //TODO
		}

	}
}
