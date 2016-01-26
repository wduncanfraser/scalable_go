#include <iostream>
#include <chrono>
#include <vector>
#include <random>

//Test for NeuralNet
#include "lib/neuralnet.h"

#define LAYERS 4
#define INPUT 32
#define HL1 40
#define HL2 10
#define OUTPUT 1
#define MUTATER 0.01

int main()
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	NeuralNet test1(LAYERS, {INPUT,HL1,HL2,OUTPUT});
	test1.initializeRandomWeights();

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "Elapsed time initializing NeuralNet: " << elapsed_seconds.count() << "s\n";

	// Setup random number generator
	std::random_device rd;
	// Generate random number as seed for twister engine
	std::mt19937 generator(rd());
	// Set bounds for real distribution
	std::uniform_real_distribution<double> distribution(-1.0, 1.0);

	std::vector<double> testInput,testInput2;
	testInput.assign(INPUT, 0);

	for(double &element: testInput)
	{
		element = distribution(generator);
	}

	//Start timing for feedforward
	start = std::chrono::system_clock::now();
	//#pragma omp parallel for firstprivate(test1)
	for(int i = 0; i < 1000000; i++)
	{
		test1.feedForward(testInput);
	}
	// End timing
	end = std::chrono::system_clock::now();

	elapsed_seconds = end-start;
	std::cout << "Elapsed time evaluating 1,000,000 iterations: " << elapsed_seconds.count() << "s\n";
	std::cout << "Iterations per second: " << 1000000/elapsed_seconds.count() << std::endl;

	std::cout << std::endl << "Testing mutator against neural network:" << std::endl;
	//Test mutator
	//Feedforwarding again as none of the feedforwards from the openMP loop are within scope.
	test1.feedForward(testInput);
	test1.displayOutput();
	test1.mutate(MUTATER);
	test1.feedForward(testInput);
	test1.displayOutput();

    //test1.display();

	std::cout<< "Testing fake board input" << std::endl;

	testInput = {1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    testInput2 = {1,1,1,0,1,0,1,1,0,1,1,1,1,0,0,0,0,-1,0,0,-1,-1,-1,0,-1,-1,-1,-1,0,-1,-1,-1};
	test1.feedForward(testInput);
	test1.displayOutput();
    test1.feedForward(testInput2);
    test1.displayOutput();

	std::ofstream output_file("testweights.txt");
    std::cout << test1.exportWeights(output_file, true) << std::endl;
	output_file.close();
	//test1.display();

	//Validate output to input
	NeuralNet test2(LAYERS, {INPUT,HL1,HL2,OUTPUT});
	std::ifstream input_file("testweights.txt");
    std::cout << test2.importWeights(input_file) << std::endl;
	//test2.display();

    if(test1 == test2)
    {
        std::cout << "Imported network is equal to exported!\n";
    }
}