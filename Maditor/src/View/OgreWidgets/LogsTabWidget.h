#pragma once

#include <qtabwidget.h>
#include <memory>
#include <qmap.h>

namespace Maditor {
	namespace Model {
		class LogsModel;
		class Log;
	}

	namespace View {

		class LogsTabWidget : public QTabWidget
		{
			Q_OBJECT

		public:
			LogsTabWidget(QWidget *parent = 0);

			void setModel(const Model::LogsModel *watcher);

		public slots:
			void addLog(Model::Log *log);
			void removeLog(Model::Log *log);

		private:
			QMap<Model::Log *, QWidget *> mTabs;

		};
	}
}

