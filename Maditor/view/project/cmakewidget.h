#ifndef MADITOR_VIEW_CMAKEWIDGET_H
#define MADITOR_VIEW_CMAKEWIDGET_H

#include <QWidget>

namespace Maditor {
	namespace View {

		namespace Ui {
			class CmakeWidget;
		}

		class CmakeWidget : public QWidget
		{
			Q_OBJECT

		public:
			explicit CmakeWidget(QWidget *parent = 0);
			~CmakeWidget();

			void setModel(Model::CmakeServer *server);
			void clearModel();

		private slots:
			void output(QString msg);

		private:
			Ui::CmakeWidget *ui;

			QTextCursor mCursor;
		};


	} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_CMAKEWIDGET_H
