#include "histogram.h"

void find_minmax(const vector<double>& numbers, double& min, double& max) {
	
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

void svg_rect(double x, double y, double width,
	double height, string stroke, string fill) {

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

