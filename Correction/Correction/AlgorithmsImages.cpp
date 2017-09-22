#include "AlgorithmsImages.h"
#include <assert.h>
#include <fstream>

#include "ImageDisplay.h"
#include "AlgorithmsCalculus.h"
#include "Parameters.h"
#include "NodesSet.h"

#include "opencv2/imgproc.hpp"

void AlgorithmsImages::createVisualImageBlocks(const cv::Mat& cvImage, int blockWidth, int blockHeight, ImageDisplay& imageDisplay)
{
	imageDisplay.raster = 1; // because we will measure everything in pixels now
	imageDisplay.blockWidth = blockWidth;
	imageDisplay.blockHeight = blockHeight;
	const int c_blocks_count_x = cvImage.cols / blockWidth + 1;
	const int c_blocks_count_y = cvImage.rows / blockHeight + 1;
	const int c_blocks_width_rest = cvImage.cols % blockWidth;
	const int c_blocks_height_rest = cvImage.rows % blockHeight;
	imageDisplay.blocksCountX = c_blocks_count_x;
	imageDisplay.blocksCountY = c_blocks_count_y;
	QVector<QImage>& imageBlocks = imageDisplay.imageBlocks;
	imageBlocks.reserve(c_blocks_count_x * c_blocks_count_y);
	for (int blockRow = 0; blockRow < c_blocks_count_y; blockRow++)
	{
		for (int blockCol = 0; blockCol < c_blocks_count_x; blockCol++)
		{
			int bWidth = (blockCol != c_blocks_count_x - 1) ? blockWidth : c_blocks_width_rest;
			int bHeight = (blockRow != c_blocks_count_y - 1) ? blockHeight : c_blocks_height_rest;
			cv::Mat cvSubImageRef(cv::Mat(cvImage, cv::Rect(blockCol * blockWidth, blockRow * blockHeight, bWidth, bHeight)));
			QImage image = Mat2QImageGray(cvSubImageRef);
			imageBlocks.push_back(image);
		}
	}
	
}

void AlgorithmsImages::findNodesApproximately(const cv::Mat& cvImage, NodesSet& nodesSet)
{
	assert(!cvImage.empty());
	if (cvImage.empty())
	{
		return;
	}
	//array of values sums of f(x,y) by y:
	std::vector<double> sumsAlongY(cvImage.cols);
	sumIntensityVertically(cvImage, sumsAlongY);

	//array of values sums of f(x,y) by X:
	std::vector<double> sumsAlongX(cvImage.rows);
	sumIntensityHorizontally(cvImage, sumsAlongX);

	//finding peaks:
	std::vector<int> peaksY;
	Algorithms::findPeaks(sumsAlongX, c_count_grid_rows, c_neighborhood, peaksY);
	std::vector<int> peaksX;
	Algorithms::findPeaks(sumsAlongY, c_count_grid_cols, c_neighborhood, peaksX);

	//finding nodes positions using finded peaks:
	bool startFromBottom = true;
	if (startFromBottom)
	{
		std::reverse(peaksY.begin(), peaksY.end());
	}

	std::vector<cv::Point> nodes;
	nodes.resize(peaksX.size() * peaksY.size());

	for (int row = 0; row < peaksY.size(); row++)
	{
		for (int col = 0; col < peaksX.size(); col++)
		{
			nodes[row * peaksX.size() + col] = cv::Point(peaksX[col], peaksY[row]);
		}
	}

	nodesSet.create(nodes, c_count_grid_cols, c_count_grid_rows);
}

void AlgorithmsImages::clarifyNodes(const cv::Mat& cvImage, NodesSet& nodesSet)
{
	// get cell size
	cv::Size cellSize = nodesSet.getCellSize();

	for (int col = 0; col < nodesSet.cols(); col++)
	{
		// create subimage of size (cellSizeX x cvImage.height) with vertical symmetry line on current column:
		int xLeft = std::max(0, (nodesSet.at(0, col).x - cellSize.width / 2));
		int xRight = std::min(cvImage.cols - 1, (nodesSet.at(0, col).x + cellSize.width / 2));
		cv::Mat cvSubImage = cv::Mat(cvImage, cv::Rect(cv::Point(xLeft, 0), cv::Point(xRight, cvImage.rows - 1)));
		
		// integrate image along X-axis:
		std::vector<double> sums;
		sumIntensityHorizontally(cvSubImage,  sums);

		//find peaks:
		std::vector<int> peaks;
		Algorithms::findPeaks(sums, c_count_grid_rows, c_neighborhood, peaks);
		std::reverse(peaks.begin(), peaks.end());

		// shift each node in column to corresponding peak
		for (int row = 0; row < nodesSet.rows(); row++)
		{
			cv::Point pNew = cv::Point(nodesSet.at(row, col).x, peaks[row]);
			cv::Point pOld = nodesSet.at(row, col);
			nodesSet.setNode(row, col, pNew);
		}
	}
}

void AlgorithmsImages::clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet)
{
	// get cell size
	cv::Size cellSize = nodesSet.getCellSize();

	for (int row = 0; row < nodesSet.rows(); row++)
	{
		for (int col = 0; col < nodesSet.cols(); col++)
		{
			//create cvSubImage of size (cellSizeX x cellSizeY / 4):
			cv::Point node = nodesSet.at(row, col);
			cv::Size sizeSubImage(cellSize.width, cellSize.height / 4);
			cv::Rect roiSubImage;
			getNodeSubImageROI(cvImage, sizeSubImage, node, roiSubImage);
			cv::Mat cvSubImage = cv::Mat(cvImage, cv::Rect(roiSubImage.tl(), roiSubImage.br()));

			//integrate subImage along Y-axis:
			std::vector<double> sums;
			sumIntensityVertically(cvSubImage,  sums);

			//find single peak:
			std::vector<int> peaks;
			Algorithms::findPeaks(sums, 1, 0, peaks);

			//shift node to finded peak:
			int nodeXLocal = node.x - roiSubImage.tl().x;
			cv::Point pNew = nodesSet.at(row, col) + cv::Point(peaks[0] - nodeXLocal, 0);
			nodesSet.setNode(row, col, pNew);
		}
	}
}


void AlgorithmsImages::sumIntensityVertically(const cv::Mat& cvImage, std::vector<double>& sums)
{
	assert(!cvImage.empty());
	sums.resize(cvImage.cols);
	std::vector<double>sumsVector;
	sumsVector.assign(cvImage.cols, 0);
#pragma omp parallel for
	for (int col = 0; col < cvImage.cols; col++)
	{
		for (int row = 0; row < cvImage.rows; row++)
		{
			sumsVector[col] += cvImage.at<uchar>(row, col);
		}
		sums[col] = (sumsVector[col] / cvImage.rows);
	}
}

void AlgorithmsImages::sumIntensityHorizontally(const cv::Mat& cvImage,  std::vector<double>& sums)
{
	assert(!cvImage.empty());
	sums.resize(cvImage.rows);
	std::vector<double>sumsVector;
	sumsVector.assign(cvImage.rows, 0);
#pragma omp parallel for
	for (int row = 0; row < cvImage.rows; row++)
	{
		for (int col = 0; col < cvImage.cols; col++)
		{
			sumsVector[row] += cvImage.at<uchar>(row, col);
		}
		sums[row] = (sumsVector[row] / cvImage.cols);
	}
}


void AlgorithmsImages::generateMask(const cv::Size& sizeLine, NodeType nodeType, cv::Mat& mask)
{
	//create matrix of size (sizeLize x 3):
	mask.create(sizeLine * 3, CV_8UC1);
	mask = cv::Scalar(0);
	//enumerate mask blocks:
	const int c_width_line = sizeLine.width;
	const int c_height_line = sizeLine.height;
	cv::Mat blockTopLeft = cv::Mat(mask, cv::Rect(0, 0, c_width_line, c_height_line));
	cv::Mat blockTop = cv::Mat(mask, cv::Rect(c_width_line, 0, c_width_line, c_height_line));
	cv::Mat blockTopRight = cv::Mat(mask, cv::Rect(2 * c_width_line, 0, c_width_line, c_height_line));
	cv::Mat blockLeft = cv::Mat(mask, cv::Rect(0, c_height_line, c_width_line, c_height_line));
	cv::Mat blockCenter = cv::Mat(mask, cv::Rect(c_width_line, c_height_line, c_width_line, c_height_line));
	cv::Mat blockRight = cv::Mat(mask, cv::Rect(2 * c_width_line, c_height_line, c_width_line, c_height_line));
	cv::Mat blockBottomLeft = cv::Mat(mask, cv::Rect(0, 2 * c_height_line, c_width_line, c_height_line));
	cv::Mat blockBottom = cv::Mat(mask, cv::Rect(c_width_line, 2 * c_height_line, c_width_line, c_height_line));
	cv::Mat blockBottomRight = cv::Mat(mask, cv::Rect(2 * c_width_line, 2 * c_height_line, c_width_line, c_height_line));
	switch (nodeType)
	{
	case CENTER:
		blockCenter = cv::Scalar(255);
		blockLeft = cv::Scalar(255);
		blockRight = cv::Scalar(255);
		blockTop = cv::Scalar(255);
		blockBottom = cv::Scalar(255);
		break;
	case TOP_LEFT:
		blockCenter = cv::Scalar(255);
		blockRight = cv::Scalar(255);
		blockBottom = cv::Scalar(255);
		break;
	case TOP_RIGHT:
		blockCenter = cv::Scalar(255);
		blockLeft = cv::Scalar(255);
		blockBottom = cv::Scalar(255);
		break;
	case BOTTOM_LEFT:
		blockCenter = cv::Scalar(255);
		blockRight = cv::Scalar(255);
		blockTop = cv::Scalar(255);
		break;
	case BOTTOM_RIGHT:
		blockCenter = cv::Scalar(255);
		blockLeft = cv::Scalar(255);
		blockTop = cv::Scalar(255);
		break;
	case LEFT:
		blockCenter = cv::Scalar(255);
		blockRight = cv::Scalar(255);
		blockTop = cv::Scalar(255);
		blockBottom = cv::Scalar(255);
		break;
	case TOP:
		blockCenter = cv::Scalar(255);
		blockLeft = cv::Scalar(255);
		blockRight = cv::Scalar(255);
		blockBottom = cv::Scalar(255);
		break;
	case RIGHT:
		blockCenter = cv::Scalar(255);
		blockLeft = cv::Scalar(255);
		blockTop = cv::Scalar(255);
		blockBottom = cv::Scalar(255);
		break;
	case BOTTOM:
		blockCenter = cv::Scalar(255);
		blockLeft = cv::Scalar(255);
		blockRight = cv::Scalar(255);
		blockTop = cv::Scalar(255);
		break;
	default:
		break;
	}

	//depends on nodeType, fill corresponding blocks of mask:
}

QImage AlgorithmsImages::Mat2QImageGray(const cv::Mat_<uchar> &src)
{
	QImage dest(src.cols, src.rows, QImage::Format_RGB32);
	for (int y = 0; y < src.rows; ++y) 
	{
		const uchar *srcrow = src[y];
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) 
		{
			const uchar color = srcrow[x];
			destrow[x] = qRgb(color, color, color);
		}
	}
	return dest;
}

void AlgorithmsImages::getLineSize(const cv::Mat& cvImage, cv::Size& size)
{
	cv::Mat cvImageBlurred;
	cv::GaussianBlur(cvImage, cvImageBlurred, cv::Size(3,3), 0, 0);

	std::vector<double> sumsAlongX;
	sumIntensityHorizontally(cvImageBlurred, sumsAlongX);
	std::vector<double> derivativesY;
	Algorithms::differentiate(sumsAlongX, derivativesY);
	std::vector<int> peaksY;
	Algorithms::findPeaks(derivativesY, 2, 3, peaksY);
	size.height = peaksY[1] - peaksY[0];

	std::vector<double> sumsAlongY;
	sumIntensityVertically(cvImageBlurred, sumsAlongY);
	std::vector<double> derivativesX;
	Algorithms::differentiate(sumsAlongY, derivativesX);
	std::vector<int> peaksX;
	Algorithms::findPeaks(derivativesX, 2, 3, peaksX);
	size.width = peaksX[1] - peaksX[0];
}

void AlgorithmsImages::findCrosshair(const cv::Mat& cvSubImage, const cv::Point& posImage,  cv::Point& crosshair)
{
	cv::Mat cvImageBlurred;
	cv::GaussianBlur(cvSubImage, cvImageBlurred, cv::Size(7, 7), 1, 1);

	std::vector<double> sumsAlongY;
	sumIntensityVertically(cvImageBlurred, sumsAlongY);
	std::vector<double> derivativesX;
	Algorithms::differentiate(sumsAlongY, derivativesX);
	std::vector<int> peaksX;
	Algorithms::findPeaks(derivativesX, 2, 3, peaksX);
	crosshair.x = (peaksX[1] + peaksX[0]) / 2 + posImage.x;

	std::vector<double> sumsAlongX;
	sumIntensityHorizontally(cvImageBlurred, sumsAlongX);
	std::vector<double> derivativesY;
	Algorithms::differentiate(sumsAlongX, derivativesY);
	std::vector<int> peaksY;
	Algorithms::findPeaks(derivativesY, 2, 3, peaksY);
	crosshair.y = (peaksY[1] + peaksY[0]) / 2 + posImage.y;
}

void AlgorithmsImages::getNodeSubImageROI(const cv::Mat& cvImage, const cv::Size& sizeROI, const cv::Point& node, cv::Rect& ROIcorrected)
{
	int left = std::max(0, node.x - sizeROI.width / 2);
	int right = std::min(cvImage.cols - 1, node.x + sizeROI.width / 2);
	int top = std::max(0, node.y - sizeROI.height / 2);
	int bottom = std::min(cvImage.rows - 1, node.y + sizeROI.height / 2);
	ROIcorrected = cv::Rect(cv::Point(left, top), cv::Point(right, bottom));
}