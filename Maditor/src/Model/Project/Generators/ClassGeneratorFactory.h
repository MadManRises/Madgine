#pragma once



namespace Maditor {
	namespace Model {
		namespace Generators {

			class ClassGeneratorFactory : public QObject {
				Q_OBJECT
			public:
				enum ClassType {
					ENTITY_COMPONENT,
					GAME_HANDLER,
					GLOBAL_API,
					GUI_HANDLER,				
					OTHER_CLASS,
					SCENE_COMPONENT					
				};

				static ClassGenerator *load(Module *module, QDomElement data);

			signals:
				void newGuiHandlerRequest(Module *module, const QString &name);
				void newGlobalAPIRequest(Module *module, const QString &name);
				void newEntityComponentRequest(Module *module, const QString &name);
				void newSceneComponentRequest(Module *module, const QString &name);
				void newGameHandlerRequest(Module *module, const QString &name);
				void newOtherClassRequest(Module *module, const QString &name);

			public slots :
				void createClass(Module *module, const QString &name, ClassType type);
				void createGuiHandler(Module *module, const QString &name, const QString &window, int type, bool hasLayoutFile);
				void createGlobalAPI(Module *module, const QString &name);
				void createEntityComponent(Module *module, const QString &name, const QString &componentName);
				void createSceneComponent(Module *module, const QString &name);
				void createGameHandler(Module *module, const QString &name);
				void createOtherClass(Module *module, const QString &name);

			};

		}
	}
}