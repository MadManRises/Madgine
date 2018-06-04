#include "maditormodellib.h"

#include "otherclassgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

			const QString OtherClassGenerator::sType = "Other";

			OtherClassGenerator::OtherClassGenerator(Module * module, const QString & name, bool headerOnly) :
				ClassGenerator(module, name, sType, headerOnly)
			{
				generate();
			}
			OtherClassGenerator::OtherClassGenerator(Module * module, QDomElement data) :
				ClassGenerator(module, data)
			{
			}
			QString OtherClassGenerator::templateFileName(int index)
			{
				if (headerOnly()) {
					return "OtherClassHeaderOnly.h";
				}
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
