
#include "gtest\gtest.h"

#include <madgineinclude.h>
#include "Model\Editor.h"

#include "mocks\DialogManagerMock.h"


using namespace ::testing;

TEST(MaditorFullTest, GenerateNewProject) {
	int argc = 0;
	QApplication app(argc, 0);
	DialogManagerMock dialogManager;

	EXPECT_CALL(dialogManager, confirmFileOverwrite(_, _))
		.WillRepeatedly(Return(true));
	
	Maditor::Model::Editor editor;
	editor.newProject("", "TestProject");

	
}