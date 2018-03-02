#pragma once

//algorithms params:
const int c_neighborhood(300);
struct Parameters
{
	int gridRows;
	int gridCols;
	double cellSizeFactor;

	int smoothingAlgorithm;

	int blurImageSize;
	int blurMaskSize;
	int maxPosError; // how much "approximate" node position can be different from "precise" (in percents).   

	bool thresholdEnabled;
	bool autoThresholdEnabled;
	int thresholdValue;

	int peakNeighGlobal;
	int peakNeighLocal;
};

enum SmoothingAlgorithm
{
	SMOOTHING_GAUSS = 0,
	SMOOTHING_MEDIAN = 1
};

