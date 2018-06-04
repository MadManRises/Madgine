#include "maditormodellib.h"

#include "globalapigenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString GlobalAPIGenerator::sType = "GlobalAPI";

			GlobalAPIGenerator::GlobalAPIGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			GlobalAPIGenerator::GlobalAPIGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString GlobalAPIGenerator::templateFileName(int index)
			{
				switch (index) {
				case 0:
					return "GlobalAPI.cpp";
				case 1:
					return "GlobalAPI.h";
				default:
					throw 0;
				}
			}
		}
	}
}
