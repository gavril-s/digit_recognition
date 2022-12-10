#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

class picture
{
private:
	int width = 0;
	int height = 0;
	std::vector<std::vector<float>> pixels;

	std::pair<std::vector<std::vector<float>>, std::pair<int, int>> read_bmp(std::istream& in);
public:
	picture();
	picture(std::istream& in, std::string img_type);

	int get_width();
	int get_height();
	std::vector<std::vector<float>> get_pixels();
	float get(int x, int y);
};

std::pair<std::vector<std::vector<float>>, std::pair<int, int>> picture::read_bmp(std::istream& in)
{
	const int HEADER_SIZE = 14;
	const int INFO_HEADER_SIZE = 40;

	unsigned char header[HEADER_SIZE];
	in.read(reinterpret_cast<char*>(header), HEADER_SIZE);

	unsigned char info_header[INFO_HEADER_SIZE];
	in.read(reinterpret_cast<char*>(info_header), INFO_HEADER_SIZE);

	int size = header[2] + (header[3] << 8) + (header[4] << 16) + (header[5] << 24);
	int res_width = info_header[4] + (info_header[5] << 8) + (info_header[6] << 16) + (info_header[7] << 24);
	int res_height = info_header[8] + (info_header[9] << 8) + (info_header[19] << 16) + (info_header[11] << 24);
	std::vector<std::vector<float>> pro_res;
	const int padding_amount = ((4 - (res_width * 3) % 4) % 4);

	for (int y = 0; y < res_height; y++)
	{
		pro_res.push_back(std::vector<float>(res_width, 0));
		for (int x = 0; x < res_width; x++)
		{
			unsigned char color[3];
			in.read(reinterpret_cast<char*>(color), 3);

			float r = static_cast<float>(color[0]);
			float g = static_cast<float>(color[1]);
			float b = static_cast<float>(color[2]);

			float avg = 1.0f - ((r + g + b) / (3 * 255));
			pro_res[y][x] = avg;
		}

		in.ignore(padding_amount);
	}

	std::vector<std::vector<float>> res(pro_res.size());
	for (int i = 0; i < pro_res.size(); i++)
	{
		res[res.size() - 1 - i] = pro_res[i];
	}

	int top = 0;
	int bottom = 0;
	int left = 0;
	int right = 0;
	bool found_non_zero = false;

	for (int i = 0; i < res_height; i++)
	{
		bool only_zeros = true;
		for (int j = 0; j < res_width; j++)
		{
			if (res[i][j] != 0)
			{
				only_zeros = false;
				found_non_zero = true;
			}
		}

		if (only_zeros && !found_non_zero)
		{
			top = i;
		}
		
		if (!only_zeros && found_non_zero)
		{
			bottom = res_height - i - 1;
		}
	}

	found_non_zero = false;
	for (int i = 0; i < res_width; i++)
	{
		bool only_zeros = true;
		for (int j = 0; j < res_height; j++)
		{
			if (res[j][i] != 0)
			{
				only_zeros = false;
				found_non_zero = true;
			}
		}

		if (only_zeros && !found_non_zero)
		{
			left = i;
		}

		if (!only_zeros && found_non_zero)
		{
			right = res_width - i - 1;
		}
	}

	while (top - bottom > 1)
	{
		for (int i = 0; i < res_height - 1; i++)
		{
			res[i] = res[i + 1];
		}
		res[res_height - 1] = std::vector<float>(res_width, 0);
		top--;
		bottom++;
	}
	while (bottom - top > 1)
	{
		for (int i = res_height - 1; i > 0; i--)
		{
			res[i] = res[i - 1];
		}
		res[0] = std::vector<float>(res_width, 0);
		bottom--;
		top++;
	}
	
	while (left - right > 1)
	{
		for (int i = 0; i < res_width - 1; i++)
		{
			for (int j = 0; j < res_height; j++)
			{
				res[j][i] = res[j][i + 1];
			}
		}

		for (int j = 0; j < res_height; j++)
		{
			res[j][res_width - 1] = 0;
		}
		left--;
		right++;
	}
	while (right - left > 1)
	{
		for (int i = res_width - 1; i > 0; i--)
		{
			for (int j = 0; j < res_height; j++)
			{
				res[j][i] = res[j][i - 1];
			}
		}

		for (int j = 0; j < res_height; j++)
		{
			res[j][0] = 0;
		}
		right--;
		left++;
	}

	return std::make_pair(res, std::make_pair(res_width, res_height));
}

picture::picture()
	: width{ 0 }, height{ 0 }, pixels{} {}

picture::picture(std::istream& in, std::string img_type)
{
	std::pair<std::vector<std::vector<float>>, std::pair<int, int>> pic_data;

	std::transform(img_type.begin(), img_type.end(), img_type.begin(),
		[](unsigned char c) { return std::toupper(c); });
	if (img_type == "BMP")
	{
		pic_data = read_bmp(in);
	}
	else
	{
		throw std::string{ "ONLY BMP" };
	}

	pixels = pic_data.first;
	width = pic_data.second.first;
	height = pic_data.second.second;
	//in.close();

	std::ofstream ofs("request.txt", std::ios::out);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofs.width(5);
			ofs << std::round(pixels[i][j] * 100) / 100;
		}
		ofs << std::endl;
	}
	ofs.close();
}

int picture::get_width()
{
	return width;
}

int picture::get_height()
{
	return height;
}

std::vector<std::vector<float>> picture::get_pixels()
{
	return std::vector<std::vector<float>>(pixels);
}

float picture::get(int x, int y)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
	{
		return -1;
	}

	return pixels[x][y];
}