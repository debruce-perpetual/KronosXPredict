#include <gtest/gtest.h>

#include "KronosXPredict/api.hpp"

TEST(TorchDemoTest, ReturnsTwoByThreeMatrix) {
    auto r = KronosXPredict::torch_demo();
    EXPECT_EQ(r.rows, 2u);
    EXPECT_EQ(r.cols, 3u);
    EXPECT_EQ(r.data.size(), r.rows * r.cols);
}
