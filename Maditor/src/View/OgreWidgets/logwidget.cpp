#include "madgineinclude.h"

#include "logwidget.h"
#include "ui_logwidget.h"
#include "Model\Engine\Watcher\LogTableModel.h"

namespace Maditor {
	namespace View {


		LogWidget::LogWidget(QWidget *parent) :
			QWidget(parent),
			ui(new Ui::LogWidget)
		{
			ui->setupUi(this);

			ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
			ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

			
		}

		LogWidget::~LogWidget()
		{
			delete ui;
		}

		void LogWidget::setModel(Model::Watcher::LogTableModel * log)
		{
			connect(ui->tableView, &QTableView::doubleClicked, log, &Model::Watcher::LogTableModel::doubleClicked);
			ui->tableView->setModel(log);
			connect(ui->clearButton, &QPushButton::clicked, log, &Model::Watcher::LogTableModel::clear);
		}

	}
}