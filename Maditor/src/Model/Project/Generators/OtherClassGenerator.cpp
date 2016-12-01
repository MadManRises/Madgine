#include "maditorlib.h"

#include "OtherClassGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString OtherClassGenerator::sType = "Other";

			OtherClassGenerator::OtherClassGenerator(Module * module, const QString & name) :
				ClassGenerator(module, name, sType)
			{
				generate();
			}
			OtherClassGenerator::OtherClassGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString OtherClassGenerator::templateFileName(int index)
			{
				switch (index) {
				case 0:
					return "OtherClass.cpp";
				case 1:
					return "OtherClass.h";
				default:
					throw 0;
				}
			}
		}
	}
}