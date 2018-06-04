#include "maditormodellib.h"

#include "document.h"

namespace Maditor {
	namespace Model {

		Document::Document(const QString &name) :
			mDirtyFlag(false),
			mName(name)
		{}

		void Document::destroy()
		{
			emit destroySignal(this);
		}

		void Document::save()
		{
			setDirtyFlag(false);
		}

		void Document::discardChanges()
		{
			setDirtyFlag(false);
		}

		bool Document::getDirtyFlag()
		{
			return mDirtyFlag;
		}

		QString Document::getQualifiedName()
		{
			return mName + (mDirtyFlag ? "*" : "");
		}

		const QString & Document::getName()
		{
			return mName;
		}

		void Document::setDirtyFlag(bool flag)
		{
			mDirtyFlag = flag;
			emit dirtyFlagChanged(flag);
		}

	}
}
