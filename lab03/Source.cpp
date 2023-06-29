#include <iostream>
#include <vector>
#include <cmath>
#include <string>


using namespace std;

const size_t SCREEN_WIDTH = 80;
const size_t MAX_ASTERISK = SCREEN_WIDTH - 3 - 1;


vector<double> input_numbers(size_t count) {
	vector<double> result(count);
	for (size_t i = 0; i < count; i++) {
		std::cin >> result[i];
	}
	return result;
}

void find_minmax(const vector<double>& numbers, double& min, double& max) {
	min = numbers[0];
	max = numbers[0];
	for (double x : numbers) {
		if (x < min) {
			min = x;
		}
		else if (x > max) {
			max = x;
		}
	}
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

void svg_begin(double width, double height) {
	cout << "<?xml version='1.0' encoding='UTF-8'?>\n";
	cout << "<svg ";
	cout << "width='" << width << "' ";
	cout << "height='" << height << "' ";
	cout << "viewBox='0 0 " << width << " " << height << "' ";
	cout << "xmlns='http://www.w3.org/2000/svg'>\n";
}

void svg_text(double left, double baseline, string text) {
	cout << "<text ";
	cout << "x = '" << left << "' ";
	cout << "y = '" << baseline << "'>";
	cout << text << "</text>";
}

void svg_rect(double x, double y, double width, double height, string stroke = "black", string fill = "black") {

	cout << "<rect ";
	cout << "x = '" << x << "' ";
	cout << "y = '" << y << "' ";
	cout << "width = '" << width << "' ";
	cout << "height = '" << height << "' ";
	cout << "stroke = '" << stroke << "' ";
	cout << "fill = '" << fill << "'";
	cout << "/> ";
}

void svg_end() {
	cout << "</svg>\n";
}

void show_histogram_svg(const vector<size_t>& bins) {
	const auto IMAGE_WIDTH = 400;
	const auto IMAGE_HEIGHT = 300;
	const auto TEXT_LEFT = 20;
	const auto TEXT_BASELINE = 20;
	const auto TEXT_WIDTH = 50;
	const auto BIN_HEIGHT = 30;
	const auto BLOCK_WIDTH = 30;

	svg_begin(IMAGE_WIDTH, IMAGE_HEIGHT);


	double top = 0;
	for (size_t bin : bins) {
		const double bin_width = BLOCK_WIDTH * bin;
		svg_text(TEXT_LEFT, top + TEXT_BASELINE, to_string(bin));
		svg_rect(TEXT_WIDTH, top, bin_width, BIN_HEIGHT, "blue", "#aaffaa");
		top += BIN_HEIGHT;
	}
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