#include "../gtest.h"
#include <vector>
#include <cmath>

/**
 * @brief this is a sample test case with Google's GTEST
 * @param sample_tests the test group's name
 * @param example_one the test's name
 */
TEST(sample_tests, example_one) {
    double x, y;
    x = 1; y = 2;
    EXPECT_TRUE(x == y);
}


