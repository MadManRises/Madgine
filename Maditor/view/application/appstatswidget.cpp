#include "maditorviewlib.h"

#include "appstatswidget.h"
#include "ui_appstatswidget.h"

#include "model/application/statsmodel.h"

namespace Maditor {
namespace View {

AppStatsWidget::AppStatsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppStatsWidget)
{
    ui->setupUi(this);
}

AppStatsWidget::~AppStatsWidget()
{
    delete ui;
}

void AppStatsWidget::setModel(Model::StatsModel * model)
{
	clearModel();

 	mCurrentConnections.emplace_back(connect(model, &Model::StatsModel::averageFPSChanged, this, &AppStatsWidget::setFPS));
	mCurrentConnections.emplace_back(connect(model, &Model::StatsModel::memoryUsageChanged, this, &AppStatsWidget::setMemUsage));
	mCurrentConnections.emplace_back(connect(model, &Model::StatsModel::ogreMemoryUsageChanged, this, &AppStatsWidget::setOgreMem));
	/*connect(ui->StartTrackAllocationsButton, &QPushButton::clicked, model, &Model::StatsModel::trackAllocations);
	connect(ui->StopTrackAllocationsButton, &QPushButton::clicked, model, &Model::StatsModel::logTrackedAllocations);*/
}

void AppStatsWidget::clearModel()
{
	for (const QMetaObject::Connection &conn : mCurrentConnections)
		disconnect(conn);
	mCurrentConnections.clear();

	setFPS(0);
	setMemUsage(0);
	setOgreMem(0);
}

void AppStatsWidget::setFPS(float fps)
{
	ui->FPSValue->setText(fps == 0.0f ? "-" : QLocale().toString(fps));
}

void AppStatsWidget::setMemUsage(int mem)
{
	ui->MemUsageValue->setText(mem == 0 ? "-" : QLocale().toString(mem / 1024));
}

void AppStatsWidget::setOgreMem(int mem)
{
	ui->OgreMemoryValue->setText(mem == 0 ? "-" : QLocale().toString(mem / 1024));
}



} // namespace View
} // namespace Maditor
