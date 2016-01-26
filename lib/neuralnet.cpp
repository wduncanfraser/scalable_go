#include <iostream>
#include <random>

#include "neuralnet.h"

//Support function for calculating activate.
//Declared inline as it is only 1 line to increase speed.
inline double activate(double x)
{
	//Using
	return ((x)/(1+std::abs(x)));
}

//Default constructor, blank
NeuralNet::NeuralNet()
{

}

//Constructor with size specifications for layers.
NeuralNet::NeuralNet(const unsigned int ilayer_count, const std::vector<unsigned int> ilayer_neuron_count)
{
	//Check that layer count is correct
	if(ilayer_count == ilayer_neuron_count.size())
	{
		layer_count = ilayer_count;
		layer_neuron_count = ilayer_neuron_count;
	}
	else
	{
		//Error generation
		std::cout << "Layer Count does not match input neuron counts.\n";
	}
	//Resize Neuron and Weight vectors to match layer_count
	neurons.resize(layer_count);
	weights.resize(layer_count-1);

	//Assign/Resize Neuron Vectors to size of neural network layers and initialize to 0.
	//Input and hidden layers +1 to account for bias
	for(unsigned int i = 0; i < layer_count; i++)
	{
		//If output layer, don't add bias
		if(i == (layer_count - 1))
		{
			neurons[i].assign(layer_neuron_count[i], 0);
		}
		//For all other layers, add bias
		else 
		{
			neurons[i].assign(layer_neuron_count[i] + 1, 0);
			//Set the bias to 1
			neurons[i][layer_neuron_count[i]] = 1.0;
		}
	}

	//Assign/Resize Weight Vectors to appropriate size [x][y] and initialize elements to 0.
	//+1 is to account for bias
	for(unsigned int i = 1; i < layer_count; i++)
	{
		weights[i-1].resize(layer_neuron_count[i]);

		for(std::vector<double> &row: weights[i-1])
		{
			row.assign(layer_neuron_count[i-1] + 1 , 0);
		}
	}
}

//Copy Constructor
NeuralNet::NeuralNet(const NeuralNet &iNetwork)
{
	//Copy layer count
	layer_count = iNetwork.layer_count;
	//Copy layer neuron counts 
	layer_neuron_count = iNetwork.layer_neuron_count;
	//Copy Neurons
	neurons = iNetwork.neurons;
	//Copy Weights
	weights = iNetwork.weights;
}

// Destructor
NeuralNet::~NeuralNet()
{
	//std::cout << "Running Destructor";
}

//Copy Operator
NeuralNet& NeuralNet::operator= (const NeuralNet &iNetwork)
{
	if(this != &iNetwork)
	{	
		//Copy layer count
		layer_count = iNetwork.layer_count;
		//Copy layer neuron counts 
		layer_neuron_count = iNetwork.layer_neuron_count;
		//Copy Neurons
		neurons = iNetwork.neurons;
		//Copy Weights
		weights = iNetwork.weights;
	}
	return *this;
}

//Comparison Operator
bool NeuralNet::operator== (const NeuralNet &iNetwork)
{
    if(weights == iNetwork.weights)
    {
        return true;
    }
    return false;
}

// Initialize Nueral Network with random weights in a uniform distribution
void NeuralNet::initializeRandomWeights()
{
	// Setup random number generator
	std::random_device rd;
	// Generate random number as seed for twister engine
	std::mt19937 generator(rd());
	// Set bounds for real distribution
	std::uniform_real_distribution<double> distribution(-1.0, 1.0);

	// Assign random values to each element in each row of weight table
	for(std::vector<std::vector<double>> &layer: weights)
	{
		for(std::vector<double> &row: layer)
		{
			for(double &element: row)
			{
				element = distribution(generator);
			}
		}
	}
}

//Export weights to specified file
int NeuralNet::exportWeights(std::ofstream &file, bool clean)
{
	if (file.is_open())
	{
        DoubleInt converter;
		if(!clean)
		{
			for(unsigned int i = 0; i < layer_count; i++)
			{
				file << "Layer " << i << " count: " << layer_neuron_count[i] << std::endl;
			}
		}
		else
		{
			file << layer_count << std::endl;

			for(unsigned int i = 0; i < layer_count; i++)
			{
				file << layer_neuron_count[i] << ",";
			}
			file << std::endl;
		}

		for(unsigned int i = 0; i < layer_count-1; i++) {
            if (!clean) {
                std::cout << "\nLayer " << i << " to layer " << i + 1 << " weights: \n";
            }

            for (std::vector<double> &row: weights[i]) {
                for (double &element: row) {
                    converter.d = element;
                    file << converter.i << ",";
                }
            }
        }
        //Newline for parsing on import
        file << std::endl;

		return 0;
	}
	else
	{
		return 1;
	}
}

int NeuralNet::importWeights(std::ifstream &file)
{
	if(!file.is_open())
	{
		std::cout << "File is not open for importWeights.\n";
		return 1;
	}

	//Import weight values
	unsigned int import_layer_count;
	std::vector<unsigned int> import_layer_neuron_count;
	//Vector to hold imported values
	std::vector<std::vector<std::vector<double>>> import_weights;

	//String and stringstream for converting data
	std::string line;

	std::istringstream layer_stream, weight_stream;

	//Get layer count
	getline(file, line, '\n');
	import_layer_count = std::stoi(line);

	if(import_layer_count != layer_count)
	{
		std::cout << "Import layer count does not match.\n";
		return 2;
	}

	import_weights.resize(layer_count-1);

	//Get layer neuron counts
	getline(file, line, '\n');
	layer_stream.str(line);

    //string to hold line elements
    std::string line_element;

	while(layer_stream)
    {
		if(!getline(layer_stream, line_element, ','))
		{
			break;
		}

		import_layer_neuron_count.push_back(std::stoi(line_element));
	}

	//Check that count matches layer count, malformed otherwise
	if(import_layer_neuron_count.size() != layer_count)
	{
		return 10;
	}

	//Check that neuron counts match
	if(import_layer_neuron_count != layer_neuron_count)
	{
		return 3;
	}

    //DoubleInt Union converter for importing doubles stored as ints
    DoubleInt converter;

    //Assign/Reize Weight Vectors to appropriate size [x][y] and initialize elements to 0.
    //+1 is to account for bias
    for(unsigned int i = 1; i < layer_count; i++)
    {
        import_weights[i-1].resize(layer_neuron_count[i]);

        for(std::vector<double> &row: import_weights[i-1])
        {
            row.assign(layer_neuron_count[i-1] + 1 , 0);
        }
    }

    //Get line with weights
    getline(file, line, '\n');
    weight_stream.str(line);

    //Import Weights
	for(unsigned int i = 0; i < layer_count-1; i++) {
        for (std::vector<double> &row: import_weights[i]) {
            for (double &element: row) {
                if(!getline(weight_stream, line_element, ',')) {
                    //Malformed, ran out of input
                    return 10;
                }
                converter.i = std::stoll(line_element);
                element = converter.d;
            }
        }
	}

	//Convert imported values to weight vectors
	weights = import_weights;

	return 0;
}

//FeedForward Function, calculate output based on inputs.
int NeuralNet::feedForward(const std::vector<double> &input)
{
	//If the size of the input vector is not the same as the Neural Network input layer, return error code.
	if (input.size() != layer_neuron_count[0])
	{
		return 1;
	}
	else
	{
		//Assign input_Neurons to match input. Last element of input_Neurons should not be touched as it is bias.
		// For each loops not being used throughout function to avoid bias nuerons and needing iterators for accessing multiple vectors.
		for(unsigned int i = 0; i < layer_neuron_count[0]; i++)
		{
			neurons[0][i] = input[i];
		}

		//Calculate through all layers
		for(unsigned int i = 1; i < layer_count; i++)
		{
			//For each neuron in each layer after input, calculate value
			for(unsigned int j = 0; j < layer_neuron_count[i]; j++)
			{
				// Reset Nueron to 0 before calculating
				neurons[i][j] = 0;

				//Sum the value of all weights*previous layer neuron value
				for(unsigned int k = 0; k <= layer_neuron_count[i-1]; k++)
				{
					neurons[i][j] += neurons[i-1][k] * weights[i-1][j][k];
				}

				//Take activate of final sum to determine value
				neurons[i][j] = activate(neurons[i][j]);
			}	
		}

		//Finished executing without error, return 0;
		return 0;
	}
}

int NeuralNet::mutate(const double &radius)
{
	// Setup random number generator
	std::random_device rd;
	// Generate random number as seed for twister engine
	std::mt19937 generator(rd());
	// Set bounds for real distribution
	std::uniform_real_distribution<double> distribution(-radius, radius);

	// Mutate each element in each row of weight tables by random values in uniform distribution
	for(std::vector<std::vector<double>> &layer: weights)
	{
		for(std::vector<double> &row: layer)
		{
			for(double &element: row)
			{
				element += distribution(generator);
			}
		}
	}

	//Finished executing without error, return 0;
	return 0;	
}

void NeuralNet::displayOutput()
{
	for(double &element: neurons[layer_count-1])
	{
		std::cout << element << " ";
	}
	std::cout << std::endl;
}

std::vector<double> NeuralNet::getOutput()
{
	return neurons[layer_count-1];
}


// Display Neural Network values. Incomplete, prototype.
// Returns 0 on success, or code on error
int NeuralNet::display()
{	
	for(unsigned int i = 0; i < layer_count; i++)
	{
		std::cout << "Layer " << i << " count: " << layer_neuron_count[i] << std::endl;
	}

	for(unsigned int i = 0; i < layer_count-1; i++)
	{
		std::cout << "Layer " << i << " to layer " << i+1 << " weights: \n";

		for(std::vector<double> &row: weights[i])
		{
			for(double &element: row)
			{
				std::cout << element << ",";
			}
			std::cout << ";";
		}
		std::cout << std::endl;
	}	

	return 0;
}