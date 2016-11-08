#include "gmock\gmock.h"

#include "View\Dialogs\DialogManager.h"

class DialogManagerMock : public Maditor::View::Dialogs::DialogManager {
public:
	MOCK_METHOD2(confirmFileOverwrite, bool(const QString &filePath, QMessageBox::StandardButton *result));
};