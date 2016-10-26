#ifndef MODULEPROPERTIESDIALOG_H
#define MODULEPROPERTIESDIALOG_H


namespace Maditor {
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
