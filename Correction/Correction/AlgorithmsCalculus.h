#pragma once
#include <vector>

namespace Algorithms
{
	/* Function returns indexes of peaks.
	parameters:
	values - input vector to find peaks,
	countPeaks - number of peaks to find,
	neighborhood - parameter needed to determine peak.
	all values in the interval [peak - neighborhood, peak + neighborhood] will be considered as
	single peak
	peaks - found peaks.
	*/
	void findPeaks(const std::vector<double>& values, int countPeaks, int neighborhood, std::vector<int>& peaks);

	void differentiate(const std::vector<double>& values, std::vector<double>& derivatives);

	double angleRadiansToDegrees(double angleRadians);
	double constrainAngle(double x);
	int sign(double x);
	bool numbersEqual(double a, double b);
	void nullifyBounds(int neighborhood, std::vector<double>& values);
}