#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

namespace Maditor {
	namespace View {

		namespace Ui {
			class LogWidget;
		}

		class LogWidget : public QWidget
		{
			Q_OBJECT

		public:
			explicit LogWidget(QWidget *parent = 0);
			~LogWidget();

			void setModel(Model::LogTableModel *log);

		private slots:
			void resize();
			void sortIndicatorChanged(int index, Qt::SortOrder order);

		private:
			Ui::LogWidget *ui;
			int mLastIndex;
		};

	}

}

#endif // LOGWIDGET_H
