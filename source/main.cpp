#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
	//-----------------------------------
	//  Startup test
	//-----------------------------------
	testing::InitGoogleTest(&argc, argv);

	int result = RUN_ALL_TESTS();

// 	std::cout << "\n Test complete, press any key to exit!" << ::std::endl;
// 	getchar();

	return result;
}