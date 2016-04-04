// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include "gtest/gtest.h"

#include "gogame.h"
#include "gogamenn.h"

TEST(gogamenn_scaling_check, scale_3_5) {
    // Validate we can scale withut issue.
    GoGameNN size3(3);
    size3.initialize_random();

    GoGameNN size5(5);

    EXPECT_NO_THROW(size5.scale_network(size3));
}

TEST(gogamenn_scaling_check, scale_5_7) {
    // Validate we can scale withut issue.
    GoGameNN size5(5);
    size5.initialize_random();

    GoGameNN size7(7);

    EXPECT_NO_THROW(size7.scale_network(size5));
}

TEST(gogamenn_scaling_check, scale_3_5_7_check_segments) {
    GoGameNN size3(3);
    size3.initialize_random();
    std::vector<NeuralNet> size_3_layer1 = size3.get_layer1();

    GoGameNN size5(5);
    size5.scale_network(size3);
    std::vector<NeuralNet> size_5_layer1 = size5.get_layer1();

    GoGameNN size7(7);
    size7.scale_network(size5);
    std::vector<NeuralNet> size_7_layer1 = size7.get_layer1();

    for (unsigned int i = 0; i < 24; i++) {
        EXPECT_EQ(size_3_layer1[0], size_7_layer1[i]);
    }

    for (unsigned int i = 0; i < 9; i++) {
        EXPECT_EQ(size_5_layer1[9], size_7_layer1[25 + i]);
    }
}
