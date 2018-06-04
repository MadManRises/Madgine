#pragma once


namespace Maditor {
	namespace Model {

class TreeItem
{
public:
	virtual ~TreeItem() = default;

	virtual int childCount() const;
	virtual int parentIndex();
	virtual TreeItem *child(int i) = 0;

	virtual TreeItem *parentItem() const = 0;

	virtual QVariant cellData(int col) const = 0;

	virtual QVariant icon() const;

	virtual void extendContextMenu(QMenu &menu);

	virtual void doubleClicked();

protected:
	void setContextMenuItems(std::list<std::pair<QString, std::function<void()>>> &&contextMenuItems);
	const std::list<std::pair<QString, std::function<void()>>> &getContextMenuItems();

private:
	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

	
};

	}
}

