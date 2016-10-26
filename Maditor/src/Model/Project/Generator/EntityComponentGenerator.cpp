#include "maditorinclude.h"

#include "EntityComponentGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

			const QString EntityComponentGenerator::sType = "EntityComponent";

			EntityComponentGenerator::EntityComponentGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			EntityComponentGenerator::EntityComponentGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString EntityComponentGenerator::templateFileName(int index)
			{
				return QString();
			}
		}
	}
}