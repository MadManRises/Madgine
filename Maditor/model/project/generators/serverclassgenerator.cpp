#include "maditormodellib.h"

#include "serverclassgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString ServerClassGenerator::sType = "Server";

			ServerClassGenerator::ServerClassGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			ServerClassGenerator::ServerClassGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString ServerClassGenerator::templateFileName(int index)
			{
				switch (index) {
				case 0:
					return "ServerClass.cpp";
				case 1:
					return "ServerClass.h";
				default:
					throw 0;
				}
			}
		}
	}
}
