#include "maditormodellib.h"

#include "gamehandlergenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

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
				switch (index) {
				case 0:
					return "GameHandler.cpp";
				case 1:
					return "GameHandler.h";
				default:
					throw 0;
				}
			}
		}
	}
}
