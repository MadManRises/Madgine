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

				QString getName();

			signals:
				void saved(ScriptEditor *editor);

			public slots:
				void save();

			private:
				QString mPath;
				int mIndex;
			};
		}
	}
}