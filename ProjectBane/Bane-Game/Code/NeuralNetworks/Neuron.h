#pragma once


#include <KieckerMath.h>
#include <vector>

class NeuralLayer;


class NeuralWeight
{
public:
	std::vector<double> NextNodeWeights;
};

class Neuron
{
public:

	void FeedForward(NeuralLayer* pLastLayer);
};

