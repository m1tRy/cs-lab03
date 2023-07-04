#pragma warning(disable : 4996)
#define INFO_BUFFER_SIZE 32767

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "histogram.h"


using namespace std;

const size_t SCREEN_WIDTH = 40;
const size_t MAX_ASTERISK = SCREEN_WIDTH - 5;


vector<double> input_numbers(size_t count) {
	vector<double> result(count);
	for (size_t i = 0; i < count; i++) {
		std::cin >> result[i];
	}
	return result;
}

vector<size_t> make_histogram(const vector<double>& numbers, size_t bin_count) {
	double min, max;
	vector<size_t> bins(bin_count);

	find_minmax(numbers, min, max);

	double bin_size = (max - min) / bin_count;

	for (size_t i = 0; i < numbers.size(); i++) {
		bool found = false;
		for (size_t j = 0; (j < bin_count - 1) && !found; j++) {
			auto lo = min + j * bin_size;
			auto hi = min + (j + 1) * bin_size;
			if ((lo <= numbers[i]) && (numbers[i] < hi)) {
				bins[j]++;
				found = true;
			}
		}
		if (!found) {
			bins[bin_count - 1]++;
		}
	}
	return bins;
}

string make_info_text() {
	stringstream buffer;

	buffer << "<tspan x = '20' dy = '1em'>";

	DWORD  info = GetVersion();
	DWORD mask_v = 0x0000ffff;
	DWORD version = info & mask_v;

	DWORD mask_M = 0x00ff;
	DWORD version_major = version & mask_M;
	DWORD version_minor = version >> 8;

	buffer << "Windows v" << version_major << "." << version_minor;

	DWORD platform = info >> 16;

	if ((info & 0x80000000) == 0) {

		DWORD build = platform;
		buffer << " (build " << build << ")\n";
	}

	buffer << "</tspan><tspan x = '20' dy = '1em'>";

	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;
	GetComputerName(infoBuf, &bufCharCount);
	wstring test(&infoBuf[0]); //convert to wstring
	string test2(test.begin(), test.end()); //and convert to string.

	buffer << "Computer name: " << test2 << "</tspan>";

	return buffer.str();
}

void show_histogram_svg(const vector<size_t>& bins) {
	const auto IMAGE_WIDTH = 400;
	const auto IMAGE_HEIGHT = 300;
	const auto TEXT_LEFT = 20;
	const auto TEXT_BASELINE = 20;
	const auto TEXT_WIDTH = 50;
	const auto BIN_HEIGHT = 30;
	const auto BLOCK_WIDTH = 10;

	size_t max_count = 0;
	bool flag_overflow = false;

	for (size_t i = 0; i < bins.size(); i++) {
		if (bins[i] > MAX_ASTERISK && bins[i] > max_count) {
			max_count = bins[i];
			flag_overflow = true;
		}
	}

	svg_begin(IMAGE_WIDTH, IMAGE_HEIGHT);


	double top = 0;
	size_t number_of_stars;
	for (size_t bin : bins) {
		number_of_stars = bin;
		if (flag_overflow) {
			number_of_stars = (int)MAX_ASTERISK * (static_cast<double>(bin) / max_count);
		}
		const double bin_width = BLOCK_WIDTH * number_of_stars;
		svg_text(TEXT_LEFT, top + TEXT_BASELINE, to_string(bin));
		svg_rect(TEXT_WIDTH, top, bin_width, BIN_HEIGHT, "blue", "#aaffaa");
		top += BIN_HEIGHT;
	}
	svg_text(TEXT_LEFT, IMAGE_HEIGHT - 50, make_info_text());
	svg_end();
}


int main() {
	size_t number_count;
	std::cerr << "Enter number count:";
	std::cin >> number_count;

	std::cerr << "Enter numbers:";
	const auto numbers = input_numbers(number_count);

	size_t bin_count;
	std::cerr << "Enter bin count:";
	std::cin >> bin_count;

	const auto bins = make_histogram(numbers, bin_count);

	show_histogram_svg(bins);
}