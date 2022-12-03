#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <algorithm>
#include "picture.h"
#include "network_node.h"

class network
{
private:
	std::vector<std::vector<network_node>> nodes;
	std::vector<std::vector<std::vector<float>>> weights;
	std::vector<std::vector<float>> biases;

	float get_random_weight();
	float get_random_bias();
	void set_weights_random();
	void set_biases_random();

	void set_input(std::vector<float> inp);
	void evaluate();
	std::vector<float> get_output();
public:
	network();
	network(std::vector<size_t> layers_sizes_);

	int recognize(std::ifstream in, std::string img_type);
};

float network::get_random_weight()
{
	return ((static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * 2) - 1;
}

float network::get_random_bias()
{
	return ((static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * 2) - 1;
}

void network::set_weights_random()
{
	for (std::vector<std::vector<float>>& i : weights)
	{
		for (std::vector<float>& j : i)
		{
			for (float& w : j)
			{
				w = get_random_weight();
			}
		}
	}
}

void network::set_biases_random()
{
	for (std::vector<float>& i : biases)
	{
		for (float& b : i)
		{
			b = get_random_bias();
		}
	}
}

void network::set_input(std::vector<float> inp)
{
	if (nodes.size() == 0 || nodes[0].size() != inp.size())
		return;

	for (int i = 0; i < min(nodes[0].size(), inp.size()); i++)
	{
		nodes[0][i].set_value(inp[i]);
	}
}

void network::evaluate()
{
	for (int i = 1; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes[i].size(); j++)
		{
			float value = biases[i][j];
			for (int k = 0; k < weights[i][j].size(); k++)
			{
				value += nodes[i - 1][k].get_value() * weights[i][j][k];
			}
			nodes[i][j].set_value(value);
			nodes[i][j].activate();
		}
	}
}

std::vector<float> network::get_output()
{
	if (nodes.size() > 0)
	{
		std::vector<float> res(nodes[nodes.size() - 1].size());
		for (int i = 0; i < res.size(); i++)
		{
			res[i] = nodes[nodes.size() - 1][i].get_value();
		}
		return res;
	}
	return std::vector<float>();
}

network::network()
{
	srand(time(NULL));
}

network::network(std::vector<size_t> layers_sizes) 
{
	srand(time(NULL));
	for (int i = 0; i < layers_sizes.size(); i++)
	{
		nodes.push_back(std::vector<network_node>(layers_sizes[i]));
		biases.push_back(std::vector<float>(layers_sizes[i], 0));
		weights.push_back(std::vector<std::vector<float>>());	
		for (int j = 0; j < layers_sizes[i]; j++)
		{
			if (i == 0)
			{
				weights[i].push_back(std::vector<float>());
			}
			else
			{
				weights[i].push_back(std::vector<float>(layers_sizes[i - 1], 0));
			}
		}
	}
	set_weights_random();
	set_biases_random();
}

int network::recognize(std::ifstream in, std::string img_type)
{
	picture pic(in, img_type);
	std::vector<std::vector<float>> pixels = pic.get_pixels();
	std::vector<float> input;

	for (std::vector<float>& row : pixels)
	{
		for (float pixel : row)
		{
			input.push_back(pixel);
		}
	}
	
	set_input(input);
	evaluate();
	std::vector<float> out = get_output();
	float result = std::distance(out.begin(), std::max_element(out.begin(), out.end()));

	for (float i : out)
		std::cout << i << ' ';
	std::cout << std::endl;

	return result;
}