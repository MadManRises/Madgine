#include "maditorlib.h"

#include "ClassGeneratorFactory.h"

#include "GuiHandlerGenerator.h"
#include "GameHandlerGenerator.h"
#include "GlobalAPIGenerator.h"
#include "SceneComponentGenerator.h"
#include "OtherClassGenerator.h"
#include "EntityComponentGenerator.h"

#include "Model\Project\Module.h"

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
				else
				{
					throw 0;
				}
			}

			void ClassGeneratorFactory::createClass(Module *module, const QString &name, ClassType type) {
				switch (type) {
				case GUI_HANDLER:
					emit newGuiHandlerRequest(module, name);
					break;
				case GAME_HANDLER:
					emit newGameHandlerRequest(module, name);
					break;
				case ENTITY_COMPONENT:
					emit newEntityComponentRequest(module, name);
					break;
				case SCENE_COMPONENT:
					emit newSceneComponentRequest(module, name);
					break;
				case GLOBAL_API:
					emit newGlobalAPIRequest(module, name);
					break;
				case OTHER_CLASS:
					emit newOtherClassRequest(module, name);
					break;
				}
			}

			void ClassGeneratorFactory::createGuiHandler(Module * module, const QString & name, const QString &window, int type, bool hasLayoutFile)
			{
				GuiHandlerGenerator *gen = new GuiHandlerGenerator(module, name, window, type, hasLayoutFile);
				module->addNewClass(gen);
			}

			void ClassGeneratorFactory::createGlobalAPI(Module * module, const QString & name)
			{
				GlobalAPIGenerator *gen = new GlobalAPIGenerator(module, name);
				module->addNewClass(gen);
			}

			void ClassGeneratorFactory::createEntityComponent(Module * module, const QString & name, const QString &componentName)
			{
				EntityComponentGenerator *gen = new EntityComponentGenerator(module, name, componentName);
				module->addNewClass(gen);
			}

			void ClassGeneratorFactory::createSceneComponent(Module * module, const QString & name)
			{
				SceneComponentGenerator *gen = new SceneComponentGenerator(module, name);
				module->addNewClass(gen);
			}

			void ClassGeneratorFactory::createGameHandler(Module * module, const QString & name)
			{
				GameHandlerGenerator *gen = new GameHandlerGenerator(module, name);
				module->addNewClass(gen);
			}

			void ClassGeneratorFactory::createOtherClass(Module * module, const QString & name)
			{
				OtherClassGenerator *gen = new OtherClassGenerator(module, name);
				module->addNewClass(gen);
			}

		}
	}
}