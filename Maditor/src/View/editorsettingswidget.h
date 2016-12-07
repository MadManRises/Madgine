#ifndef MADITOR_VIEW_EDITORSETTINGSWIDGET_H
#define MADITOR_VIEW_EDITORSETTINGSWIDGET_H

#include <QWidget>

#include "Dialogs\settingstab.h"

namespace Maditor {
namespace View {

namespace Ui {
class EditorSettingsWidget;
}

class EditorSettingsWidget : public Dialogs::SettingsTab
{
    Q_OBJECT

public:
    explicit EditorSettingsWidget(Model::Editor *editor);
    ~EditorSettingsWidget();

	// Inherited via SettingsTab
	virtual void setup() override;
	virtual bool apply() override;

private:
    Ui::EditorSettingsWidget *ui;

	Model::Editor *mEditor;
};


} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_EDITORSETTINGSWIDGET_H
