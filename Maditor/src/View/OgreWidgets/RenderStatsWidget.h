#pragma once

#include <qwidget.h>
#include <qtimer.h>
#include <OgreRenderTarget.h>

namespace Ui {
	class RenderStatsWidget;
}

namespace Maditor {

	namespace Model {
		namespace Watcher {
			class ApplicationWatcher;
		}
	}

	namespace View {

		class RenderStatsWidget : public QWidget
		{
			Q_OBJECT

		public:
			RenderStatsWidget(QWidget *parent = 0);
			~RenderStatsWidget();

			void connectWatchers(const QString &name, const Model::Watcher::ApplicationWatcher &watcher);

		public slots:
			void setStats(const QString &name, const Ogre::RenderTarget::FrameStats *stats);
			void update();

		private:
			Ui::RenderStatsWidget *ui;

			QTimer mTimer;
			const Ogre::RenderTarget::FrameStats *mStats;
			QString mName;
		};
	}
}

