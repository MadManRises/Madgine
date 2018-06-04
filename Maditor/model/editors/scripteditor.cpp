#include "maditorlib.h"

#include "ScriptEditor.h"

#include "Qsci\qscilexercpp.h"

#include "ScriptAPI.h"


namespace Maditor {
	namespace Model {
		namespace Editors {


			ScriptEditor::ScriptEditor(const QString &path) :
				mIndex(-1),
				mPath(path)
			{
				setObjectName(path);

				QFile f(path);
				if (!f.open(QFile::ReadOnly | QFile::Text))
					throw 0;
				QTextStream stream(&f);
				QString text = stream.readAll();
				f.close();

				setText(text);
				setBraceMatching(QsciScintilla::SloppyBraceMatch);
				setMatchedBraceForegroundColor(QColor(Qt::red));
				setMarginLineNumbers(0, true);
				setMarginType(0, QsciScintilla::NumberMargin);
				setMarginWidth(0, "***");

				setTabWidth(4);
				QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
				setFont(font);
				//setFolding(QsciScintilla::PlainFoldStyle);
				setFolding(QsciScintilla::CircledTreeFoldStyle);
				setAutoIndent(true);
				setAutoCompletionThreshold(2);
				setAutoCompletionSource(QsciScintilla::AcsAPIs);


				QsciLexer *lexer = new QsciLexerCPP();
				lexer->setDefaultFont(font);
				lexer->setAPIs(new ScriptAPI(lexer));
				setLexer(lexer);

				connect(new QShortcut(Qt::CTRL + Qt::Key_S, this), &QShortcut::activated, this, &ScriptEditor::save);
			}

			void ScriptEditor::save() {
				setModified(false);

				QFile file(mPath);
				if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
				{
					throw 0;
				}
				QTextStream stream(&file);

				stream << text();

				stream.flush();
				file.flush();
				file.close();

				emit saved(this);

			}

			int ScriptEditor::index()
			{
				return mIndex;
			}

			void ScriptEditor::setIndex(int index)
			{
				mIndex = index;
			}

			QString ScriptEditor::getName()
			{
				QFileInfo info(mPath);
				return info.fileName();
			}

		}
	}
}