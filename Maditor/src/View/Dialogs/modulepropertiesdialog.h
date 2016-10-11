#ifndef MODULEPROPERTIESDIALOG_H
#define MODULEPROPERTIESDIALOG_H

#include <QDialog>



namespace Maditor {
	namespace Model {
		class Module;
	}

	namespace View {
		namespace Dialogs {

			namespace Ui {
				class ModulePropertiesDialog;
			}

class ModulePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModulePropertiesDialog(Model::Module *module);
    ~ModulePropertiesDialog();

public slots:
	bool apply();

private:
	Model::Module *mModule;

    Ui::ModulePropertiesDialog *ui;
};


		}
	}
}

#endif // MODULEPROPERTIESDIALOG_H
