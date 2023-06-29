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