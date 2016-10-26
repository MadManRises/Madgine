#pragma once

namespace Maditor {
	namespace Model {
		namespace Editors {

			class ScriptAPI : public QsciAbstractAPIs {
				Q_OBJECT
				
			public:
				ScriptAPI(QsciLexer *lexer);


				// Inherited via QsciAbstractAPIs
				virtual void updateAutoCompletionList(const QStringList & context, QStringList & list) override;

				virtual QStringList callTips(const QStringList & context, int commas, QsciScintilla::CallTipsStyle style, QList<int>& shifts) override;

			};
		}
	}
}