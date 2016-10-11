#include "ClassGenerator.h"
#include "GameHandlerGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

			const QString GameHandlerGenerator::sType = "GameHandler";

			GameHandlerGenerator::GameHandlerGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			GameHandlerGenerator::GameHandlerGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString GameHandlerGenerator::templateFileName(int index)
			{
				return QString();
			}
		}
	}
}