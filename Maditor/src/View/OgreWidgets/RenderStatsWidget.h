#pragma once

#include "StatsInfo.h"

namespace Ui {
	class RenderStatsWidget;
}


namespace Maditor {

	namespace View {

		class RenderStatsWidget : public QWidget
		{
			Q_OBJECT

		public:
			RenderStatsWidget(QWidget *parent = 0);
			~RenderStatsWidget();

			void connectWatchers(const QString &name, const Model::Watcher::ApplicationWatcher &watcher);

		public slots:
			void setStats(const QString &name, const FrameStats &stats);
			void update();

		private:
			Ui::RenderStatsWidget *ui;

			QTimer mTimer;
			FrameStats mStats;
			QString mName;
		};
	}
}

