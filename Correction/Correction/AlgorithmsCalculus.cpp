#include "AlgorithmsCalculus.h"
#include <algorithm>
#define _USE_MATH_DEFINES // for C++  
#include <math.h>  
#include <assert.h>



void Algorithms::findPeaks(const std::vector<double>& values, int countPeaks, int neighborhood, std::vector<int>& peaks)
{
	peaks.reserve(countPeaks);
	std::vector<double> valuesWork(values);
#pragma omp parallel for
	for (int i = 0; i < valuesWork.size(); i++)
	{
		valuesWork[i] = fabs(valuesWork[i]);
	}
	for (int i = 0; i < countPeaks; i++)
	{
		//find maximum:
		auto iterPeak = std::max_element(valuesWork.begin(), valuesWork.end());
		int indexOfPeak = std::distance(valuesWork.begin(), iterPeak);

		//save index of maximum:
		peaks.push_back(indexOfPeak);

		//delete maximum (set it to 0 or minimum):
		int boundLeft(std::max(0, indexOfPeak - neighborhood));
		int boundRight(std::min(static_cast<int>(values.size() - 1), indexOfPeak + neighborhood));
		for (int j = boundLeft; j <= boundRight; j++)
		{
			valuesWork[j] = 0;
		}
	}
	std::sort(peaks.begin(), peaks.end());
}

void Algorithms::differentiate(const std::vector<double>& values, std::vector<double>& derivatives)
{
	derivatives.assign(values.size(), 0);
#pragma omp parallel for
	for (int i = 1; i < derivatives.size() - 1; i++)
	{
		derivatives[i] = (values[i + 1] - values[i - 1]) / 2;
	}
}

double Algorithms::angleRadiansToDegrees(double angleRadians)
{
	return  angleRadians * (180.0 / M_PI);
}


double Algorithms::constrainAngle(double angle) {
	angle = fmod(angle + 180, 360);
	if (angle < 0)
		angle += 360;
	return angle - 180;
}

int Algorithms::sign(double x)
{
	if (numbersEqual(x, 0))
	{
		return 1;
	}
	return (x > 0) - (x < 0);
}

bool Algorithms::numbersEqual(double a, double b)
{
	double eps = 1E-8;
	return fabs(a - b) < eps;
}

void Algorithms::nullifyBounds(int neighborhood, std::vector<double>& values)
{
	const int count_elems = values.size();
	assert(2 * neighborhood < count_elems);
	for (int i = 0; i < neighborhood; i++)
	{
		values[i] = 0;
		values[count_elems - 1 - i] = 0;
	}
}
