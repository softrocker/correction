#include "AlgorithmsCalculus.h"
#include <algorithm>

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
