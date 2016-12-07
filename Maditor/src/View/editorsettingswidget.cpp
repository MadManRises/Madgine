#include "maditorlib.h"

#include "editorsettingswidget.h"
#include "ui_editorsettingswidget.h"

#include "Model\Editor.h"

namespace Maditor {
namespace View {

EditorSettingsWidget::EditorSettingsWidget(Model::Editor *editor) :
    ui(new Ui::EditorSettingsWidget),
	mEditor(editor)
{
    ui->setupUi(this);
}

EditorSettingsWidget::~EditorSettingsWidget()
{
    delete ui;
}

void EditorSettingsWidget::setup()
{
	ui->reloadBox->setChecked(mEditor->reloadProjectProperty());
}

bool EditorSettingsWidget::apply()
{
	mEditor->setReloadProjectProperty(ui->reloadBox->isChecked());
	return true;
}

} // namespace View
} // namespace Maditor
