#include <iostream>
#include <vector>
#include <cmath>

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

void show_histogram_text(const vector<size_t>& bins) {
	size_t max_count = 0;
	bool flag_overflow = false;

	for (size_t i = 0; i < bins.size(); i++) {
		if (bins[i] > MAX_ASTERISK && bins[i] > max_count) {
			max_count = bins[i];
			flag_overflow = true;
		}
	}

	for (size_t i = 0; i < bins.size(); i++) {
		int number_of_stars = bins[i];

		if (number_of_stars < 10) {
			cout << "  ";
		}
		else if (number_of_stars < 100) {
			cout << " ";
		}
		cout << number_of_stars << "|";
		if (flag_overflow) {
			number_of_stars = (int)MAX_ASTERISK * (static_cast<double>(bins[i]) / max_count);
		}
		for (size_t j = 0; j < number_of_stars; j++) {
			cout << "*";
		}
		cout << endl;

	}
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

	show_histogram_text(bins);
}