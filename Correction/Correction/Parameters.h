#pragma once

//application params:
//const int c_count_grid_cols(33);
//const int c_count_grid_rows(33);

//algorithms params:
const int c_neighborhood(300);
//const double M_PI = 3.14159265359;

struct Parameters
{
	int gridRows;
	int gridCols;
	bool valid()
	{
		return (gridRows > 0) && (gridRows <= 65) && (gridCols > 0) && (gridCols <= 65);
	}
};