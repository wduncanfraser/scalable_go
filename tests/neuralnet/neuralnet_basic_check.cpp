// Copyright [2016] <duncan@wduncanfraser.com>

#include <vector>
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"

#include "neuralnet.h"

#define LAYERS 4
#define INPUT 32
#define HL1 40
#define HL2 10
#define OUTPUT 1
#define MUTATER 0.01

TEST(neuralnet_basic_check, input_difference) {
    // Check that different inputs result in different outputs on the same network
    NeuralNet test1(LAYERS, {INPUT, HL1, HL2, OUTPUT});
    test1.initialize_random();

    std::vector<double> test_input1, test_input2;

    test_input1 = {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                   -1, -1};
    test_input2 = {1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, 0, -1,
                   -1, -1};

    test1.feed_forward(test_input1);
    std::vector<double> output1 = test1.get_output();
    test1.feed_forward(test_input2);
    std::vector<double> output2 = test1.get_output();

    EXPECT_NE(output1, output2);
}

TEST(neuralnet_basic_check, mutator_mutates) {
    // Check that the mutator actually changes the network
    NeuralNet test1(LAYERS, {INPUT, HL1, HL2, OUTPUT});
    test1.initialize_random();

    NeuralNet test2(test1);
    test2.mutate(MUTATER);

    EXPECT_NE(test1, test2);
}

TEST(neuralnet_basic_check, mutator_output) {
    // Check that output changes after mutator
    NeuralNet test1(LAYERS, {INPUT, HL1, HL2, OUTPUT});
    test1.initialize_random();

    NeuralNet test2(test1);
    test2.mutate(MUTATER);

    std::vector<double> test_input;
    test_input = {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                  -1, -1};

    test1.feed_forward(test_input);
    test2.feed_forward(test_input);

    EXPECT_NE(test1.get_output(), test2.get_output());
}

TEST(neuralnet_basic_check, write_to_file) {
    // Check that there is no data loss/roundng error in exporting the network to file
    NeuralNet test1(LAYERS, {INPUT, HL1, HL2, OUTPUT});
    test1.initialize_random();

    std::ofstream output_file("testweights.txt");
    test1.export_weights_stream(output_file);
    output_file.close();

    NeuralNet test2(LAYERS, {INPUT, HL1, HL2, OUTPUT});
    std::ifstream input_file("testweights.txt");
    test2.import_weights_stream(input_file);
    input_file.close();

    EXPECT_EQ(test1, test2);
}
