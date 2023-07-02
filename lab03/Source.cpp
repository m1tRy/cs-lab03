#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
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

Input read_input(std::istream& in, bool prompt) {
	Input data;

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
		svg_rect(TEXT_WIDTH + SHIFT_CAPT, top, bin_width, BIN_HEIGHT, "blue", "#aaffaa");
		top += BIN_HEIGHT;

		if (i < bins.size() - 1) {
			std::string string_signature = make_nice_sign(input.column_signature[i]);
			svg_text(TEXT_LEFT, top + TEXT_BASELINE, string_signature);
			top += BIN_HEIGHT;
		}
	}
	svg_end();
}

size_t write_data(void* items, size_t item_size, size_t item_count, void* ctx) {
	size_t data_size = item_size * item_count;
	const char* char_items = reinterpret_cast<const char*>(items);
	std::stringstream* buffer = reinterpret_cast<std::stringstream*>(ctx);
	buffer->write(char_items, data_size);
	return data_size;
}

Input download(const std::string& address) {
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
	return read_input(buffer, false);
}


int main(int argc, char* argv[]) {
	Input input;
		if (argc > 1) {
		input = download(argv[1]);
	}
	else {
		input = read_input(std::cin, true);
	}

	const auto bins = make_histogram(input);
	show_histogram_svg(bins, input);
}