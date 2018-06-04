#include "maditorviewlib.h"

#include "newguihandlerdialog.h"
#include "ui_newguihandlerdialog.h"


namespace Maditor {
	namespace View {
		namespace Dialogs {

			NewGuiHandlerDialog::NewGuiHandlerDialog(QWidget *parent) :
				QDialog(parent),
				ui(new Ui::NewGuiHandlerDialog)
			{
				ui->setupUi(this);
			}

			NewGuiHandlerDialog::~NewGuiHandlerDialog()
			{
				delete ui;
			}

			QString NewGuiHandlerDialog::window()
			{
				return ui->windowField->text();
			}

			int NewGuiHandlerDialog::type()
			{
				return ui->typeField->currentIndex();
			}

			bool NewGuiHandlerDialog::layoutFile()
			{
				return ui->layoutField->isChecked();
			}

		}
	}
}