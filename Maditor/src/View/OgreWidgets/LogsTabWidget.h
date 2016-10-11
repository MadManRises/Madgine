#pragma once

#include <qtabwidget.h>
#include <memory>

namespace Maditor {
	namespace Model {
		namespace Watcher {
			class LogsWatcher;
			class LogWatcher;
		}
	}

	namespace View {

		class LogsTabWidget : public QTabWidget
		{
			Q_OBJECT

		public:
			LogsTabWidget(QWidget *parent = 0);

			void connectWatchers(const Model::Watcher::LogsWatcher *watcher);

		public slots:
			void addLogWatcher(Model::Watcher::LogWatcher *log);

		};
	}
}

