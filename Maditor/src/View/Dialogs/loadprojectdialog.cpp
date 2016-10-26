#include "maditorinclude.h"

#include "loadprojectdialog.h"
#include "ui_loadprojectdialog.h"

namespace Maditor {
	namespace View {

		LoadProjectDialog::LoadProjectDialog(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::LoadProjectDialog)
		{
			ui->setupUi(this);
		}

		LoadProjectDialog::~LoadProjectDialog()
		{
			delete ui;
		}

		QString LoadProjectDialog::path()
		{
			return QString();
		}

	}
}