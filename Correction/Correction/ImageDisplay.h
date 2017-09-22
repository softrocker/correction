#pragma once
#include <QImage>

struct ImageDisplay
{
	QVector<QImage> imageBlocks;
	int blockWidth;
	int blockHeight;
	int blocksCountX;
	int blocksCountY;
	int raster;
};