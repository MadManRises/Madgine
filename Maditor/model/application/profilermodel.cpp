#include "maditormodellib.h"

#include "profilermodel.h"

namespace Maditor {
	namespace Model {

		ProfilerModel::ProfilerModel() :
			TreeUnit(4)
		{
			setContainer(mTopLevelItems);
		}

		ProfilerItem::ProfilerItem(ProfilerModel * parent, const std::string & name) :
			TreeUnitItem(parent),
			mName(name),
			mParent(nullptr),
			mDuration(0),
			mFullDuration(0.0f),
			mUpdateSlot(this)
		{
			setContainer(mChildren);
			mDuration.setCallback(mUpdateSlot);
		}

		ProfilerItem::ProfilerItem(ProfilerItem * parent, const std::string & name) :
			TreeUnitItem(parent),
			mName(name),
			mParent(parent),
			mDuration(0),
			mFullDuration(0.0f),
			mUpdateSlot(this)
		{
			setContainer(mChildren);
			mDuration.setCallback(mUpdateSlot);
		}

		int ProfilerItem::childCount() const
		{
			return mChildren.size();
		}

		ProfilerItem * ProfilerItem::child(int i)
		{
			auto it = mChildren.begin();
			std::advance(it, i);
			return &*it;
		}

		const std::string & ProfilerItem::key() const
		{
			return mName;
		}

		QVariant ProfilerItem::cellData(int col) const
		{
			switch (col) {
			case 0:
				return QString::fromStdString(mName);
			case 1:
				return QLocale().toString(mDuration / 20000000.f);
			case 2:
				return mParent && mParent->mDuration > 0 ? 100.f * mDuration / mParent->mDuration : QVariant("-");
			case 3:
				return 100.f * mDuration / mFullDuration;
			}

			return QVariant();
		}

		std::tuple<ProfilerItem*, std::string> ProfilerItem::createNode(const std::string & name)
		{
			return{ this, name };
		}

		void ProfilerItem::update(size_t fullDuration)
		{
			mFullDuration = fullDuration;
			notify();
			//notifyDataChange(3);
			for (ProfilerItem &child : mChildren) {
				child.update(fullDuration);
			}
		}

		void ProfilerItem::notify()
		{
			notifyDataChange(1, 3);
		}

		int ProfilerModel::childCount() const
		{
			return mTopLevelItems.size();
		}

		ProfilerItem * ProfilerModel::child(int i)
		{
			auto it = mTopLevelItems.begin();
			std::advance(it, i);
			return &*it;
		}

		void ProfilerModel::reset()
		{
			mTopLevelItems.clear();
		}

		QVariant ProfilerModel::header(int col) const
		{
			switch (col) {
			case 0:
				return "Name";
			case 1:
				return "Total Time (ms)";
			case 2:
				return "Rel. Time (parent)";
			case 3:
				return "Rel. Time (total)";
			default:
				return QVariant();
			}
		}

		std::tuple<ProfilerModel*, std::string> ProfilerModel::createNode(const std::string & name)
		{
			return{ this, name };
		}

	}
}
