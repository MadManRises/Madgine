#include "newotherclassdialog.h"
#include "ui_newotherclassdialog.h"

namespace Maditor {
namespace View {
	namespace Dialogs {

		NewOtherClassDialog::NewOtherClassDialog(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::NewOtherClassDialog)
		{
			ui->setupUi(this);
		}

		NewOtherClassDialog::~NewOtherClassDialog()
		{
			delete ui;
		}

		bool NewOtherClassDialog::headerOnly()
		{
			return ui->headerOnlyBox->isChecked();
		}

	}
} // namespace View
} // namespace Maditor
