#pragma once
#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct Input {
	vector<double> numbers;
	size_t bin_count;
};

void find_minmax(const vector<double>& numbers, double& min, double& max);

void svg_begin(double width, double height);

void svg_text(double left, double baseline, string text);

void svg_rect(double x, double y, double width, double height, string stroke = "black", string fill = "black");

void svg_end();

