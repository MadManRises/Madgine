#include <gmock/gmock.h>
#include "model/dialogmanager.h"

namespace Maditor {
namespace Model {

class MockDialogManager : public DialogManager {
 public:
  MOCK_METHOD2(confirmFileOverwrite,
      bool(const QString &filePath, QMessageBox::StandardButton *result));
  MOCK_METHOD0(settingsDialog,
      View::Dialogs::SettingsDialog*());
  MOCK_METHOD2(showNewProjectDialog,
      bool(QString &path, QString &name));
  MOCK_METHOD1(showLoadProjectDialog,
      bool(QString &path));
  MOCK_METHOD2(showNewConfigDialog,
      bool(Model::ConfigList *list, QString &name));
  MOCK_METHOD2(showNewModuleDialog,
      bool(Model::ModuleList *list, QString &name));
  MOCK_METHOD3(showNewClassDialog,
      bool(Model::Module *module, QString &name, Model::Generators::ClassGeneratorFactory::ClassType &type));
  MOCK_METHOD5(showNewGuiHandlerDialog,
      bool(Model::Module *module, const QString &name, QString &window, int &type, bool &hasLayout));
  MOCK_METHOD2(showNewGlobalAPIDialog,
      bool(Model::Module *module, const QString &name));
  MOCK_METHOD3(showNewEntityComponentDialog,
      bool(Model::Module *module, const QString &name, QString &componentName));
  MOCK_METHOD2(showNewSceneComponentDialog,
      bool(Model::Module *module, const QString &name));
  MOCK_METHOD2(showNewGameHandlerDialog,
      bool(Model::Module *module, const QString &name));
  MOCK_METHOD3(showNewOtherClassDialog,
      bool(Model::Module *module, const QString &name, bool &headerOnly));
  MOCK_METHOD2(showNewServerClassDialog,
      bool(Model::Module *module, const QString &name));
  MOCK_METHOD0(showSettingsDialog,
      void());
  MOCK_METHOD2(showDeleteClassDialog,
      bool(Generators::ClassGenerator *generator, bool &deleteFiles));
};

}  // namespace Model
}  // namespace Maditor
