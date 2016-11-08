#include "madgineinclude.h"

#include "RenderStatsWidget.h"
#include "Model\Engine\Watcher\ApplicationWatcher.h"

#include "ui_renderstatswidget.h"

namespace Maditor {
	namespace View {

		RenderStatsWidget::RenderStatsWidget(QWidget *parent) :
			QWidget(parent),
			ui(new Ui::RenderStatsWidget),
			mStats(0){
			ui->setupUi(this);
			connect(&mTimer, &QTimer::timeout, this, &RenderStatsWidget::update);
			mTimer.start(0);
		}
		

		RenderStatsWidget::~RenderStatsWidget()
		{
			delete ui;
		}

		void RenderStatsWidget::connectWatchers(const QString &name, const Model::Watcher::ApplicationWatcher & watcher)
		{
			mName = name;
			ui->groupBox->setTitle(name);
			connect(&watcher, &Model::Watcher::ApplicationWatcher::renderStatsSetup, this, &RenderStatsWidget::setStats);
		}

		void RenderStatsWidget::update()
		{
			if (mStats) {
				ui->FPS->setText(QVariant(mStats->avgFPS).toString().mid(0, 6));
				ui->BatchCount->setText(QVariant(mStats->batchCount).toString());
				ui->TriangleCount->setText(QVariant(mStats->triangleCount).toString());
			}
			else {
				ui->FPS->setText("-");
				ui->BatchCount->setText("-");
				ui->TriangleCount->setText("-");
			}
		}

		void RenderStatsWidget::setStats(const QString &name, const Ogre::RenderTarget::FrameStats *stats) {
			if (name == mName)
				mStats = stats;
		}

	}

}

