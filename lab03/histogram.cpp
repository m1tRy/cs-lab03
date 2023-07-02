#include "histogram.h"

void find_minmax(const std::vector<double>& numbers, double& min, double& max) {
	
	if (numbers.size() != 0) {

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

}

void svg_begin(double width, double height) {
	std::cout << "<?xml version='1.0' encoding='UTF-8'?>\n";
	std::cout << "<svg ";
	std::cout << "width='" << width << "' ";
	std::cout << "height='" << height << "' ";
	std::cout << "viewBox='0 0 " << width << " " << height << "' ";
	std::cout << "xmlns='http://www.w3.org/2000/svg'>\n";
}

void svg_text(double left, double baseline, std::string text) {
	std::cout << "<text ";
	std::cout << "x = '" << left << "' ";
	std::cout << "y = '" << baseline << "'>";
	std::cout << text << "</text>";
}

void svg_rect(double x, double y, double width,
	double height, std::string stroke, std::string fill) {

	std::cout << "<rect ";
	std::cout << "x = '" << x << "' ";
	std::cout << "y = '" << y << "' ";
	std::cout << "width = '" << width << "' ";
	std::cout << "height = '" << height << "' ";
	std::cout << "stroke = '" << stroke << "' ";
	std::cout << "fill = '" << fill << "'";
	std::cout << "/> ";
}

void svg_end() {
	std::cout << "</svg>\n";
}

