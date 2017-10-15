#pragma once

//algorithms params:
const int c_neighborhood(300);

struct Parameters
{
	int gridRows;
	int gridCols;
	double cellSizeFactor; 
	int blurImage;
	int blurMask;
	bool valid()
	{
		return (gridRows > 0) && (gridRows <= 65) && (gridCols > 0) && (gridCols <= 65);
	}
};