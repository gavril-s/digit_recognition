#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include "picture.h"
#include "network_node.h"

class network
{
private:
	const char* train_dataset_path = "../network/source/mnist/mnist_train.csv";
	const char* test_dataset_path  = "../network/source/mnist/mnist_test.csv";
	const char* weights_path = "weights.txt";
	static const int default_eras_number = 5;
	const int train_items = 10000;
	const int test_items = 100;
	static const bool default_do_train = false;
	const float learning_rate = 0.5f;

	std::vector<std::vector<network_node>> nodes;
	std::vector<std::vector<std::vector<float>>> weights;
	std::vector<std::vector<float>> biases;

	void dump_weights();
	void load_weights();

	float get_random_weight();
	float get_random_bias();
	void set_weights_random();
	void set_biases_random();

	void set_input(std::vector<float> inp);
	void feed_forward();
	std::vector<float> get_output();

	void train(std::ifstream data, int repeat);
	float test();
	float back_propagation(std::ifstream& data);

public:
	network();
	network(std::vector<size_t> layers_sizes, bool do_train = default_do_train, int train_repeat = default_eras_number);

	int recognize(std::ifstream in, std::string img_type);
};

void network::dump_weights()
{
	std::ofstream ofs(weights_path);

	for (std::vector<std::vector<float>>& i : weights)
	{
		for (std::vector<float>& j : i)
		{
			for (float& w : j)
			{
				ofs << w << ' ';
			}
		}
	}
}

void network::load_weights()
{
	std::ifstream ifs(weights_path);

	for (std::vector<std::vector<float>>& i : weights)
	{
		for (std::vector<float>& j : i)
		{
			for (float& w : j)
			{
				ifs >> w;
			}
		}
	}
}

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

void network::feed_forward()
{
	for (int i = 1; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes[i].size(); j++)
		{
			float value = 0;// biases[i][j];
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

float network::test()
{
	std::ifstream data(test_dataset_path);

	std::string line;
	std::getline(data, line);

	int pic_count = 0;
	int right = 0;
	int all = 0;

	while (std::getline(data, line))
	{
		pic_count++;
		if (pic_count > test_items)
		{
			break;
		}

		std::vector<std::vector<float>> pixels(28, std::vector<float>(28, 0));
		int right_answer = 0;
		std::stringstream pic_stream(line);

		std::getline(pic_stream, line, ',');
		right_answer = std::stoi(line);

		int i = 0;
		while (std::getline(pic_stream, line, ','))
		{
			int val = std::stoi(line);
			pixels[i / pixels.size()][i % pixels.size()] = val / 255.0f;
			i++;
		}

		std::vector<float> input;
		for (std::vector<float>& row : pixels)
		{
			for (float pixel : row)
			{
				input.push_back(pixel);
			}
		}
		set_input(input);
		feed_forward();
		std::vector<float> out = get_output();
		int result = std::distance(out.begin(), std::max_element(out.begin(), out.end()));

		if (result == right_answer)
		{
			right++;
		}
		all++;
	}

	return (float)right / (float)all;
}

void network::train(std::ifstream data, int repeat)
{
	std::cout << "Training network..." << std::endl;
	std::cout << "(there will be " << repeat << " eras total)" << std::endl;
	for (int i = 0; i < repeat; i++)
	{
		auto start = std::chrono::high_resolution_clock::now();

		float accuracy = back_propagation(data);

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		float seconds = duration.count() / 1000.0f;

		std::cout << "Era #";
		std::cout.width(std::to_string(repeat - 1).length());
		std::cout << i << ' ';
		for (int j = 0; j < 24 - std::to_string(repeat - 1).length(); j++) std::cout << '=';
		std::cout << std::endl;
		std::cout << "Accuracy: " << accuracy * 100 << '%'
				  << std::endl
				  << "Time: " << std::round(seconds * 100) / 100 << 's'
				  << std::endl;
		for (int j = 0; j < 30; j++) std::cout << '=';
		std::cout << std::endl;
	}
}

float network::back_propagation(std::ifstream& data)
{
	int right = 0;
	int all = 0;
	int pic_count = 0;

	std::string line;
	std::getline(data, line);

	while (std::getline(data, line))
	{
		pic_count++;
		if (pic_count > train_items)
		{
			break;
		}

		std::vector<std::vector<float>> pixels(28, std::vector<float>(28, 0));
		int right_answer = 0;
		std::stringstream pic_stream(line);

		std::getline(pic_stream, line, ',');
		right_answer = std::stoi(line);

		int i = 0;
		while (std::getline(pic_stream, line, ','))
		{
			int val = std::stoi(line);
			pixels[i / pixels.size()][i % pixels.size()] = val / 255.0f;
			i++;
		}

		std::vector<float> input;
		for (std::vector<float>& row : pixels)
		{
			for (float pixel : row)
			{
				input.push_back(pixel);
			}
		}
		set_input(input);
		feed_forward();
		std::vector<float> out = get_output();
		int result = std::distance(out.begin(), std::max_element(out.begin(), out.end()));

		if (result == right_answer)
		{
			right++;
		}
		all++;

		std::vector<std::vector<float>> delta;
		for (int i = 0; i < nodes.size(); i++)
		{
			delta.push_back(std::vector<float>(nodes[i].size(), 0));
		}
		
		// output layer
		for (int k = 0; k < out.size(); k++)
		{
			float t = (k == right_answer ? 1 : 0);
			delta[delta.size() - 1][k] = -out[k] * (1 - out[k]) * (t - out[k]);
		}

		for (int l = delta.size() - 2; l >= 0; l--)
		{
			for (int j = 0; j < delta[l].size(); j++)
			{
				for (int k = 0; k < delta[l + 1].size(); k++)
				{
					delta[l][j] += delta[l + 1][k] * weights[l + 1][k][j];
				}

				float val = nodes[l][j].get_value();
				delta[l][j] *= val * (1 - val);
			}
		}

		for (int l = 1; l < weights.size(); l++)
		{
			for (int j = 0; j < weights[l].size(); j++)
			{
				for (int i = 0; i < weights[l][j].size(); i++)
				{
					weights[l][j][i] -= learning_rate * delta[l][j] * nodes[l - 1][i].get_value();
				}
			}
		}
	}
	
	dump_weights();
	return test();//(float)right / (float)all;
}

network::network()
{
	srand(time(NULL));
}

network::network(std::vector<size_t> layers_sizes, bool do_train, int train_repeat)
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
	if (do_train)
	{
		train(std::ifstream(train_dataset_path), train_repeat);
	}
	else
	{
		load_weights();
	}
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
	feed_forward();
	std::vector<float> out = get_output();
	int result = std::distance(out.begin(), std::max_element(out.begin(), out.end()));

	for (float i : out)
		std::cout << i << ' ';
	std::cout << std::endl;

	return result;
}