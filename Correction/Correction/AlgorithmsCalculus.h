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

	/* Function returns vector of derivatives of the input real values */
	void differentiate(const std::vector<double>& values, std::vector<double>& derivatives);

	/* Funcfion nullifies values in the start and the end of vector to remove parasite peaks */
	void nullifyBounds(int neighborhood, std::vector<double>& values);

	/* Function transforms angle in radians to angle in degrees */
	double angleRadiansToDegrees(double angleRadians); // 

	/* Function returns sign of x. 
	(x < 0) --> return -1
	(x equals 0) --> return 0
	(x > 0) --> return 1
	*/
	int sign(double x); 

	/* Function checks if |a - b| < eps for small epsilon */
	bool numbersEqual(double a, double b); 

}