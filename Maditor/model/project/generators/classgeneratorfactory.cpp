#include "maditormodellib.h"

#include "classgeneratorfactory.h"

#include "guihandlergenerator.h"
#include "gamehandlergenerator.h"
#include "globalapigenerator.h"
#include "scenecomponentgenerator.h"
#include "otherclassgenerator.h"
#include "entitycomponentgenerator.h"
#include "serverclassgenerator.h"

#include "model/project/module.h"

#include "dialogmanager.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			ClassGenerator * ClassGeneratorFactory::load(Module * module, QDomElement data)
			{
				QString type = data.attribute("type");
				if (type == EntityComponentGenerator::sType) {
					return new EntityComponentGenerator(module, data);
				}
				else if (type == GuiHandlerGenerator::sType) {
					return new GuiHandlerGenerator(module, data);
				}
				else if (type == GlobalAPIGenerator::sType) {
					return new GlobalAPIGenerator(module, data);
				}
				else if (type == SceneComponentGenerator::sType) {
					return new SceneComponentGenerator(module, data);
				}
				else if (type == OtherClassGenerator::sType) {
					return new OtherClassGenerator(module, data);
				}
				else if (type == GameHandlerGenerator::sType) {
					return new GameHandlerGenerator(module, data);
				}
				else if (type == ServerClassGenerator::sType) {
					return new ServerClassGenerator(module, data);
				}
				else
				{
					throw 0;
				}
			}

			ClassGenerator *ClassGeneratorFactory::createClass(Module *module) {
				ClassGeneratorFactory::ClassType type;
				QString name;
				if (DialogManager::showNewClassDialogStatic(module, name, type)) {
					switch (type) {
					case GUI_HANDLER:
					{
						QString window;
						int type;
						bool hasLayout;
						if (DialogManager::showNewGuiHandlerDialogStatic(module, name, window, type, hasLayout)) {
							return new Generators::GuiHandlerGenerator(module, name, window, type, hasLayout);
						}
					}
					break;
					case Generators::ClassGeneratorFactory::GAME_HANDLER:
						if (DialogManager::showNewGameHandlerDialogStatic(module, name)) {
							return new Generators::GameHandlerGenerator(module, name);
						}
						break;
					case Generators::ClassGeneratorFactory::ENTITY_COMPONENT:
					{
						QString componentName;
						if (DialogManager::showNewEntityComponentDialogStatic(module, name, componentName)) {
							return new Generators::EntityComponentGenerator(module, name, componentName);
						}
					}
					break;
					case Generators::ClassGeneratorFactory::SCENE_COMPONENT:
						if (DialogManager::showNewSceneComponentDialogStatic(module, name)) {
							return new Generators::SceneComponentGenerator(module, name);
						}
						break;
					case Generators::ClassGeneratorFactory::GLOBAL_API:
						if (DialogManager::showNewGlobalAPIDialogStatic(module, name)) {
							return new Generators::GlobalAPIGenerator(module, name);
						}
						break;
					case Generators::ClassGeneratorFactory::OTHER_CLASS:
					{
						bool headerOnly;
						if (DialogManager::showNewOtherClassDialogStatic(module, name, headerOnly)) {
							return new Generators::OtherClassGenerator(module, name, headerOnly);
						}
					}
					break;
					case Generators::ClassGeneratorFactory::SERVER_CLASS:
						if (DialogManager::showNewServerClassDialogStatic(module, name)) {
							return new Generators::ServerClassGenerator(module, name);
						}
					default:
						throw 0;
					}
				}
				return nullptr;
			}


		}
	}
}
