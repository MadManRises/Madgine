#ifndef MADITOR_VIEW_NEWOTHERCLASSDIALOG_H
#define MADITOR_VIEW_NEWOTHERCLASSDIALOG_H

#include <QDialog>

namespace Maditor {
namespace View {
	namespace Dialogs {

		namespace Ui {
			class NewOtherClassDialog;
		}

		class NewOtherClassDialog : public QDialog
		{
			Q_OBJECT

		public:
			explicit NewOtherClassDialog(QWidget *parent = 0);
			~NewOtherClassDialog();

			bool headerOnly();

		private:
			Ui::NewOtherClassDialog *ui;
		};

	}
} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_NEWOTHERCLASSDIALOG_H
