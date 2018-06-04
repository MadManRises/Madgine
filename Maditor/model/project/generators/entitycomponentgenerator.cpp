#include "maditormodellib.h"

#include "entitycomponentgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString EntityComponentGenerator::sType = "EntityComponent";

			EntityComponentGenerator::EntityComponentGenerator(Module * module, const QString & name, const QString &componentName) :
				ClassGenerator(module, name, sType)
			{
				element().setAttribute("componentName", componentName);

				init();

				generate();
			}
			EntityComponentGenerator::EntityComponentGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
				init();
			}
			QString EntityComponentGenerator::templateFileName(int index)
			{
				switch (index) {
				case 0:
					return "EntityComponent.cpp";
				case 1:
					return "EntityComponent.h";
				default:
					throw 0;
				}
			}
			void EntityComponentGenerator::init()
			{
				setKeys({
					{"componentName", stringAttribute("componentName")}
				});
			}
		}
	}
}
