
#include <gtest/gtest.h>

#include "model/maditormodellib.h"
#include "model/maditor.h"

#include "mocks/dialogmanagermock.h"


using namespace ::testing;

TEST(MaditorFullTest, GenerateNewProject) {
	int argc = 0;
	//QApplication app(argc, 0);
	Maditor::Model::MockDialogManager dialogManager;

	EXPECT_CALL(dialogManager, confirmFileOverwrite(_, _))
		.WillRepeatedly(Return(true));
	
	//Maditor::Model::Maditor editor;
	//editor.newProject("", "TestProject");

	
}
