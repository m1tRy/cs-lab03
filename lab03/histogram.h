#pragma once
#include <vector>
#include <string>
#include <iostream>


struct Input {
	std::vector<double> numbers;
	size_t bin_count;
};

void find_minmax(const std::vector<double>& numbers, double& min, double& max);

void svg_begin(double width, double height);

void svg_text(double left, double baseline, std::string text);

void svg_rect(double x, double y, double width, double height, std::string stroke = "black", std::string fill = "black");

void svg_end();

