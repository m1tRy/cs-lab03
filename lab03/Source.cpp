#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <cmath>
#include <string>
#include "histogram.h"


using namespace std;

const size_t SCREEN_WIDTH = 40;
const size_t MAX_ASTERISK = SCREEN_WIDTH - 5;

vector<double> input_numbers(istream& in, size_t count) {
	vector<double> result(count);
	for (size_t i = 0; i < count; i++) {
		in >> result[i];
	}
	return result;
}

Input read_input(istream& in, bool prompt) {
	Input data;

	if(prompt)
		cerr << "Enter number count: ";
	size_t number_count;
	in >> number_count;

	if (prompt)
		cerr << "Enter numbers: ";
	data.numbers = input_numbers(in, number_count);

	if (prompt)
		cerr << "Enter bin count:";
	size_t bin_count;
	cin >> data.bin_count;

	return data;
}

vector<size_t> make_histogram(Input input) {
	double min, max;
	vector<size_t> bins(input.bin_count);

	find_minmax(input.numbers, min, max);

	double bin_size = (max - min) / input.bin_count;

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
		svg_rect(TEXT_WIDTH, top, bin_width, BIN_HEIGHT, "green", "#aaffaa");
		top += BIN_HEIGHT;
	}
	svg_end();
}


int main(int argc, char* argv[]) {
	if (argc > 1) {

		CURL* curl = curl_easy_init();
		if (curl) {
			CURLcode res;
			curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

			/*отключение проаерки SSL-сертификата (https://filesamples.com/samples/document/txt/sample3.txt)*/
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			res = curl_easy_perform(curl);
			cout << res;

			curl_easy_cleanup(curl);
		}

		return 0;

	}
	//curl_global_init(CURL_GLOBAL_ALL);

	const auto input = read_input(cin, true);

	const auto bins = make_histogram(input);

	show_histogram_svg(bins);
}