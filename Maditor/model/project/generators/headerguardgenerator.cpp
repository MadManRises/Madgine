#include "maditormodellib.h"

#include "model/project/module.h"
#include "headerguardgenerator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			HeaderGuardGenerator::HeaderGuardGenerator(Module * module, const QString & name) :
				mModule(module),
				mName(name)
			{
			}

			QStringList HeaderGuardGenerator::filePaths()
			{
				return{ mModule->path() + fileName() };
			}

			void HeaderGuardGenerator::write(QTextStream & stream, int index)
			{
				stream << templateFile("HeaderGuard.h").arg(mName, guardName());
			}

			const QString & HeaderGuardGenerator::name()
			{
				return mName;
			}

			QString HeaderGuardGenerator::fileName()
			{
				return fileName(mName);
			}

			QString HeaderGuardGenerator::guardName()
			{
				return guardName(mName);
			}

			QString HeaderGuardGenerator::fileName(const QString & name)
			{
				return name + "_module.h";
			}

			QString HeaderGuardGenerator::guardName(const QString & name)
			{
				return name.toUpper() + "_EXPORT";
			}


		}
	}
}
