#ifndef NEWGUIHANDLERDIALOG_H
#define NEWGUIHANDLERDIALOG_H


namespace Maditor {
	namespace View {
		namespace Dialogs {

			namespace Ui {
				class NewGuiHandlerDialog;
			}

			class NewGuiHandlerDialog : public QDialog
			{
				Q_OBJECT

			public:
				explicit NewGuiHandlerDialog(QWidget *parent = 0);
				~NewGuiHandlerDialog();

				int type();
				bool layoutFile();
				QString window();

			private:
				Ui::NewGuiHandlerDialog *ui;
			};

		}
	}
}

#endif // NEWGUIHANDLERDIALOG_H
