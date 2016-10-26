#ifndef LOADPROJECTDIALOG_H
#define LOADPROJECTDIALOG_H

namespace Ui {
	class LoadProjectDialog;
}

namespace Maditor {

	namespace View {

		

		class LoadProjectDialog : public QDialog
		{
			Q_OBJECT

		public:
			explicit LoadProjectDialog(QWidget *parent = 0);
			~LoadProjectDialog();

			QString path();

		private:
			Ui::LoadProjectDialog *ui;
		};
	}
}

#endif // LOADPROJECTDIALOG_H
