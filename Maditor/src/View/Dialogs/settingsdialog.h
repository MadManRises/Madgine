#pragma once 



namespace Maditor {

	namespace View {
		namespace Dialogs {

			namespace Ui {
				class SettingsDialog;
			}

			class SettingsTab;

class MADITOR_EXPORT SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog();
    ~SettingsDialog();

	void addSettingsTab(SettingsTab *tab, const QString &title);

	void open();

public slots:
	bool apply();

private:

    Ui::SettingsDialog *ui;

	std::list<SettingsTab*> mTabs;
};


		}
	}
}
