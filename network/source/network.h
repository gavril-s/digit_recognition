#pragma once
#include <fstream>

class network
{
private:
public:
	network();
	int recognize(std::ifstream in);
};

network::network()
{

}

int network::recognize(std::ifstream in)
{
	return 5;
}