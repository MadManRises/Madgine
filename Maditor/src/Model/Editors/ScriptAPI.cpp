#include "maditorlib.h"

#include "ScriptAPI.h"

namespace Maditor {
	namespace Model {
		namespace Editors {


			ScriptAPI::ScriptAPI(QsciLexer *lexer) :
				QsciAbstractAPIs(lexer) {
				
			}

			void ScriptAPI::updateAutoCompletionList(const QStringList & context, QStringList & list)
			{
				qDebug() << context;
				if (false /*inData*/) {

				} else if (false /*inMethod*/) {
					list << "my.";
				}
				else if (false /*inEntity*/) {

				}
				else {
					list << "entity" << "data";
				}
			}

			QStringList ScriptAPI::callTips(const QStringList & context, int commas, QsciScintilla::CallTipsStyle style, QList<int>& shifts)
			{

				qDebug() << context << commas;

				QStringList result;

				result << "Test";
				shifts << 5;

				return result;
			}

		}
	}
}