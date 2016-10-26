#pragma once


namespace Maditor {
	namespace Model {
		namespace Editors {

			class ScriptEditor : public QsciScintilla {
				Q_OBJECT
				
			public:
				ScriptEditor(const QString &path);

				int index();
				void setIndex(int index);

			public slots:
				void save();

			private:
				QString mPath;
				int mIndex;
			};
		}
	}
}