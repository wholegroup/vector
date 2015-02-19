#include "stdafx.h"

using ::testing::Throw;

using namespace ::com::wholegroup::vector;

//
class VectorPsTest
	: public ::testing::Test
{
};

//
TEST_F(VectorPsTest, thread)
{
	fclose(fopen("/tmp/threadIsWork.TXT", "w"));
}
