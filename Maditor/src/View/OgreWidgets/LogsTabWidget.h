#pragma once


namespace Maditor {
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

