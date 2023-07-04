#pragma warning(disable : 4996)
#define INFO_BUFFER_SIZE 32767

#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "histogram.h"


const size_t SCREEN_WIDTH = 40;
const size_t MAX_ASTERISK = SCREEN_WIDTH - 5 - 2;

std::vector<double> input_numbers(std::istream& in, size_t count) {
	std::vector<double> result(count);
	for (size_t i = 0; i < count; i++) {
		in >> result[i];
	}
	return result;
}

Input read_input(std::istream& in, bool prompt, Input &data) {

	if (prompt)
		std::cerr << "Enter number count: ";
	size_t number_count;
	in >> number_count;

	if (prompt)
		std::cerr << "Enter numbers: ";
	std::vector<double> result(number_count);
	for (size_t i = 0; i < number_count; i++) {
		in >> result[i];
	}
	data.numbers = result;

	if (prompt)
		std::cerr << "Enter bin count:";
	size_t bin_count;
	in >> data.bin_count;

	return data;
}

std::vector<size_t> make_histogram(Input &input) {
	double min, max;
	std::vector<size_t> bins(input.bin_count);

	find_minmax(input.numbers, min, max);

	double bin_size = (max - min) / input.bin_count;

	std::vector<double> column_signature_local(input.bin_count - 1);
	for (size_t i = 0; i < input.bin_count - 1; i++)
	{
		column_signature_local[i] = min + (i + 1) * bin_size;
	}
	input.column_signature = column_signature_local;

	for (size_t i = 0; i < input.numbers.size(); i++) {
		bool found = false;
		for (size_t j = 0; (j < input.bin_count - 1) && !found; j++) {
			auto lo = min + j * bin_size;
			auto hi = min + (j + 1) * bin_size;
			if ((lo <= input.numbers[i]) && (input.numbers[i] < hi)) {
				bins[j]++;
				found = true;
			}
		}
		if (!found) {
			bins[input.bin_count - 1]++;
		}
	}
	return bins;
}

std::string make_nice_sign(double number) {
	std::string string_num = std::to_string(round(number * 100) / 100);
	return string_num.erase(string_num.find(".") + 3);
}


std::string make_info_text() {

	std::stringstream buffer;

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
	std::wstring test(&infoBuf[0]); //convert to wstring
	std::string test2(test.begin(), test.end()); //and convert to string.

	buffer << "Computer name: " << test2 << "</tspan>";

	return buffer.str();
}

void show_histogram_svg(const std::vector<size_t>& bins, Input input) {

	const auto IMAGE_WIDTH = 400;
	const auto IMAGE_HEIGHT = 300;
	const auto TEXT_LEFT = 20;
	const auto TEXT_BASELINE = 20;
	const auto TEXT_WIDTH = 50;
	const auto BIN_HEIGHT = 30;
	const auto BLOCK_WIDTH = 10;
	const auto SHIFT_CAPT = 20;

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
	for (size_t i = 0; i < bins.size(); i++) {
		number_of_stars = bins[i];
		if (flag_overflow) {
			number_of_stars = (int)MAX_ASTERISK * (static_cast<double>(bins[i]) / max_count);
		}
		const double bin_width = BLOCK_WIDTH * number_of_stars;
		svg_text(TEXT_LEFT + SHIFT_CAPT, top + TEXT_BASELINE, std::to_string(bins[i]));
		svg_rect(TEXT_WIDTH + SHIFT_CAPT, top, bin_width, BIN_HEIGHT, input.options[1], input.options[0]);
		top += BIN_HEIGHT;

		if (i < bins.size() - 1) {
			std::string string_signature = make_nice_sign(input.column_signature[i]);
			svg_text(TEXT_LEFT, top + TEXT_BASELINE, string_signature);
			top += BIN_HEIGHT;
		}
	}
	svg_text(TEXT_LEFT, IMAGE_HEIGHT - 50, make_info_text());
	svg_end();
}

size_t write_data(void* items, size_t item_size, size_t item_count, void* ctx) {
	size_t data_size = item_size * item_count;
	const char* char_items = reinterpret_cast<const char*>(items);
	std::stringstream* buffer = reinterpret_cast<std::stringstream*>(ctx);
	buffer->write(char_items, data_size);
	return data_size;
}

Input download(const std::string& address, Input &input) {
	std::stringstream buffer;

	CURL* curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
		/*отключение проаерки SSL-сертификата*/
		/*https://m1try.github.io/txt_marks/marks.txt*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cout << curl_easy_strerror(res);
			exit(1);
		}
		curl_easy_cleanup(curl);
	}
	return read_input(buffer, false, input);
}


Input input_commands(int argc, char* argv[], Input &input) {

	std::vector<std::string> erroes;
	int const NUMBER_OF_ARG = 6;
	int const NUMBER_OF_OPT = 3;
	if (argc > NUMBER_OF_ARG) {
		std::cerr << "Enter comand like: <progr.exe> --fill red --stroke green https://..." << std::endl;
		std::cerr << "or" << std::endl;
		std::cerr << "Enter comand like: <progr.exe> -f red -s green https://..." << std::endl;
		exit(1);
	}

	std::vector<std::string> vector_argv;
	for (size_t i = 1; i < argc; i++)
	{
		vector_argv.push_back(argv[i]);
	}

	for (size_t j = 0; j < NUMBER_OF_OPT - 1; j++) {
		for (size_t i = 0; i < vector_argv.size(); i++) {

			std::string arg = vector_argv[i];
			if (arg == "--fill" || arg == "-f") {
				if (i != vector_argv.size() - 1 && vector_argv[i + 1][0] != '-') {
					auto iter = vector_argv.cbegin();
					input.options[0] = vector_argv[i + 1];
					vector_argv.erase(iter + i);
					iter = vector_argv.cbegin();
					vector_argv.erase(iter + i);
					break;
				}
				else {
					erroes.push_back("Enter comand like: --fill red or -f red");
					auto iter = vector_argv.cbegin();
					vector_argv.erase(iter + i);
				}
			}
			if (arg == "--stroke" || arg == "-s") {
				if (i != vector_argv.size() - 1 && vector_argv[i + 1][0] != '-') {
					auto iter = vector_argv.cbegin();
					input.options[1] = vector_argv[i + 1];
					vector_argv.erase(iter + i);
					iter = vector_argv.cbegin();
					vector_argv.erase(iter + i);
					break;

				}
				else {
					erroes.push_back("Enter comand like: --stroke red or -s red");
					auto iter = vector_argv.cbegin();
					vector_argv.erase(iter + i);
				}
			}
		}

	}

	if (vector_argv.size() > 1) {
		std::cerr << "Enter comand like: <progr.exe> --fill red --stroke green https://..." << std::endl;
		std::cerr << "or" << std::endl;
		std::cerr << "Enter comand like: <progr.exe> -f red -s green https://..." << std::endl;
		exit(1);
	}


	if (erroes.size() != 0) {
		std::cerr << "error:" << std::endl;
		for (size_t i = 0; i < erroes.size(); i++)
		{
			std::cerr << erroes[i] << std::endl;
		}
		exit(1);
	}

	std::string path;
	if (vector_argv.size() == 1) {
		return download(vector_argv[0], input);
	}
	else {
		return read_input(std::cin, true, input);
	}
}

int main(int argc, char* argv[]) {
	Input input;
	if (argc > 1) {
		input = input_commands(argc, argv, input);
	}
	else {
		input = read_input(std::cin, true, input);
	}

	const auto bins = make_histogram(input);
	show_histogram_svg(bins, input);
}