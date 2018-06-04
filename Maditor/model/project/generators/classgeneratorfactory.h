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
					SCENE_COMPONENT,
					SERVER_CLASS
				};

				static ClassGenerator *load(Module *module, QDomElement data);

				static ClassGenerator *createClass(Module *module);
			};

		}
	}
}