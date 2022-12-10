#pragma once
#include <vector>
#include <math.h>

class network_node
{
private:
	float value;

	float activation_func(float val);
	float activation_func_deriv(float val);

public:
	network_node();

	void activate();

	void set_value(float val);
	float get_value();
};

network_node::network_node() 
	: value{ 0 } {}

void network_node::activate()
{
	value = activation_func(value);
}

void network_node::set_value(float val)
{
	value = val;
}

float network_node::get_value()
{
	return value;
}

float network_node::activation_func(float val)
{
	return 1.0f / (1.0f + exp(-val));
}

float network_node::activation_func_deriv(float val)
{
	return activation_func(val) * (1 - activation_func(val));
}