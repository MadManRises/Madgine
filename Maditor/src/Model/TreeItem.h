#pragma once


namespace Maditor {
	namespace Model {

class TreeItem
{
public:
	virtual ~TreeItem() = default;

	virtual int childCount() = 0;
	virtual int parentIndex();
	virtual TreeItem *child(int i) = 0;

	virtual TreeItem *parentItem() = 0;

	virtual QVariant data(int col) const = 0;

	virtual QVariant icon() const;

	virtual void extendContextMenu(QMenu &menu);

	virtual void doubleClicked();

protected:
	void setContextMenuItems(std::list<std::pair<QString, std::function<void()>>> &&contextMenuItems);


private:
	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

	
};

	}
}

