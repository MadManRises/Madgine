#include "maditorinclude.h"

#include "OtherClassGenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

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
				return QString();
			}
		}
	}
}