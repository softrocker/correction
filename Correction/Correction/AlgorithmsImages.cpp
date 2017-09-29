#include "AlgorithmsImages.h"
#include <assert.h>
#include <fstream>

#include "ImageDisplay.h"
#include "AlgorithmsCalculus.h"

#include "NodesSet.h"
#include "DataTransfer.h"

#include <QMessageBox>

#include "opencv2/imgproc.hpp"

#define _USE_MATH_DEFINES // for C++  
#include <math.h> 

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

//void AlgorithmsImages::findNodesApproximately(const cv::Mat& cvImage, NodesSet& nodesSet, int grid_rows, int grid_cols)
//{
//	assert(!cvImage.empty());
//	if (cvImage.empty())
//	{
//		return;
//	}
//	//array of values sums of f(x,y) by y:
//	std::vector<double> sumsAlongY(cvImage.cols);
//	sumIntensityVertically(cvImage, sumsAlongY);
//
//	//array of values sums of f(x,y) by X:
//	std::vector<double> sumsAlongX(cvImage.rows);
//	sumIntensityHorizontally(cvImage, sumsAlongX);
//
//	//finding peaks:
//	std::vector<int> peaksY;
//	Algorithms::findPeaks(sumsAlongX, grid_rows, c_neighborhood, peaksY);
//	std::vector<int> peaksX;
//	Algorithms::findPeaks(sumsAlongY, grid_cols, c_neighborhood, peaksX);
//
//	//finding nodes positions using finded peaks:
//	bool startFromBottom = true;
//	if (startFromBottom)
//	{
//		std::reverse(peaksY.begin(), peaksY.end());
//	}
//
//	std::vector<cv::Point> nodes;
//	nodes.resize(peaksX.size() * peaksY.size());
//
//	for (int row = 0; row < peaksY.size(); row++)
//	{
//		for (int col = 0; col < peaksX.size(); col++)
//		{
//			nodes[row * peaksX.size() + col] = cv::Point(peaksX[col], peaksY[row]);
//		}
//	}
//
//	nodesSet.create(nodes, grid_cols, grid_rows);
//}

//void AlgorithmsImages::clarifyNodes(const cv::Mat& cvImage, NodesSet& nodesSet)
//{
//	// get cell size
//	cv::Size cellSize = nodesSet.getCellSize();
//
//	for (int col = 0; col < nodesSet.cols(); col++)
//	{
//		// create subimage of size (cellSizeX x cvImage.height) with vertical symmetry line on current column:
//		int xLeft = std::max(0, (nodesSet.at(0, col).x - cellSize.width / 2));
//		int xRight = std::min(cvImage.cols - 1, (nodesSet.at(0, col).x + cellSize.width / 2));
//		cv::Mat cvSubImage = cv::Mat(cvImage, cv::Rect(cv::Point(xLeft, 0), cv::Point(xRight, cvImage.rows - 1)));
//		
//		// integrate image along X-axis:
//		std::vector<double> sums;
//		sumIntensityHorizontally(cvSubImage,  sums);
//
//		//find peaks:
//		std::vector<int> peaks;
//		Algorithms::findPeaks(sums, nodesSet.rows(), c_neighborhood, peaks);
//		std::reverse(peaks.begin(), peaks.end());
//
//		// shift each node in column to corresponding peak
//		for (int row = 0; row < nodesSet.rows(); row++)
//		{
//			cv::Point pNew = cv::Point(nodesSet.at(row, col).x, peaks[row]);
//			cv::Point pOld = nodesSet.at(row, col);
//			nodesSet.setNode(row, col, pNew);
//		}
//	}
//}

//void AlgorithmsImages::clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet)
//{
//	// get cell size
//	cv::Size cellSize = nodesSet.getCellSize();
//
//	for (int row = 0; row < nodesSet.rows(); row++)
//	{
//		for (int col = 0; col < nodesSet.cols(); col++)
//		{
//			//create cvSubImage of size (cellSizeX x cellSizeY / 4):
//			cv::Point node = nodesSet.at(row, col);
//			cv::Size sizeSubImage(cellSize.width, cellSize.height / 4);
//			cv::Rect roiSubImage;
//			getNodeSubImageROI(cvImage, sizeSubImage, node, roiSubImage);
//			cv::Mat cvSubImage = cv::Mat(cvImage, cv::Rect(roiSubImage.tl(), roiSubImage.br()));
//
//			//integrate subImage along Y-axis:
//			std::vector<double> sums;
//			sumIntensityVertically(cvSubImage,  sums);
//
//			//find single peak:
//			std::vector<int> peaks;
//			Algorithms::findPeaks(sums, 1, 0, peaks);
//
//			//shift node to finded peak:
//			int nodeXLocal = node.x - roiSubImage.tl().x;
//			cv::Point pNew = nodesSet.at(row, col) + cv::Point(peaks[0] - nodeXLocal, 0);
//			nodesSet.setNode(row, col, pNew);
//		}
//	}
//}


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

void AlgorithmsImages::sumIntensityHorizontallyWithSlope(const cv::Mat& cvImage, double angleRadians, std::vector<double>& sums)
{
	const int c_count = cvImage.rows + cvImage.cols;
	sums.resize(c_count);
	for (int i = 0; i < c_count; i++)
	{
		// find 2 points, between which we will draw line:
		int rho = -i;
		double theta = M_PI / 2 - angleRadians;
		double a = cos(theta);
		double b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		cv::Point pt1(cvRound(x0 + 1000 * (-b)),
			cvRound(y0 + 1000 * (a)));
		cv::Point pt2(cvRound(x0 - 1000 * (-b)),
			cvRound(y0 - 1000 * (a)));

		// sum points intensity along line:
		cv::LineIterator it(cvImage, pt1, pt2, 8);
		for (int k = 0; k < it.count; k++, ++it)
		{
			int value = cvImage.at<uchar>(it.pos());
			sums[i] += value;
		}
		//sums[i] /= it.count;
		if (it.count != 0)
		{
			sums[i] /= it.count;
		}
		else
		{
			sums.resize(i);
			break;
		}
		
	}
}


void AlgorithmsImages::generateMask(const cv::Size& sizeImage, const cv::Size& sizeLine, NodeType nodeType, cv::Mat& mask)
{
	//create matrix of size (sizeLize x 3):
	mask.create(sizeImage, CV_8UC1);
	mask = cv::Scalar(0);

	//enumerate mask blocks:
	const int c_width_image = sizeImage.width;
	const int c_height_image = sizeImage.height;
	const int c_width_line = sizeLine.width;
	const int c_height_line = sizeLine.height;

	cv::Size sizeBlockEmpty = cv::Size((c_width_image - c_width_line) / 2, (c_height_image - c_height_line) / 2);
	cv::Mat blockTopLeft = cv::Mat(mask, cv::Rect(0, 0, sizeBlockEmpty.width, sizeBlockEmpty.height));
	cv::Mat blockTop = cv::Mat(mask, cv::Rect(sizeBlockEmpty.width, 0, c_width_line, sizeBlockEmpty.height));
	cv::Mat blockTopRight = cv::Mat(mask, cv::Rect(sizeBlockEmpty.width + c_width_line, 0, sizeBlockEmpty.width, sizeBlockEmpty.height));
	cv::Mat blockLeft = cv::Mat(mask, cv::Rect(0, sizeBlockEmpty.height, sizeBlockEmpty.width, c_height_line));
	cv::Mat blockCenter = cv::Mat(mask, cv::Rect(sizeBlockEmpty.width, sizeBlockEmpty.height, c_width_line, c_height_line));
	cv::Mat blockRight = cv::Mat(mask, cv::Rect(sizeBlockEmpty.width + c_width_line, sizeBlockEmpty.height, sizeBlockEmpty.width, c_height_line));
	cv::Mat blockBottomLeft = cv::Mat(mask, cv::Rect(0, sizeBlockEmpty.height + c_height_line, sizeBlockEmpty.width, sizeBlockEmpty.height));
	cv::Mat blockBottom = cv::Mat(mask, cv::Rect(sizeBlockEmpty.width, sizeBlockEmpty.height + c_height_line, c_width_line, sizeBlockEmpty.height));
	cv::Mat blockBottomRight = cv::Mat(mask, cv::Rect(sizeBlockEmpty.width + c_width_line, sizeBlockEmpty.height + c_height_line, sizeBlockEmpty.width, sizeBlockEmpty.height));
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


void AlgorithmsImages::generateMaskWithAngle(const cv::Size& sizeImage, const cv::Size& sizeLine, double angleRadians, NodeType nodeType, cv::Mat& mask)
{
	//create matrix of size (sizeLize x 3):
	mask.create(sizeImage, CV_8UC1);
	mask = cv::Scalar(0);

	cv::Point pCenter(sizeImage.width / 2, sizeImage.height / 2);

	//fill vertical lines:
	for (int i = 0; i < sizeLine.width; i++)
	{
		cv::Point p1 = cv::Point(pCenter.x - (sizeLine.width / 2) + i, 0); // point on top
		cv::Point p2 = cv::Point(pCenter.x - (sizeLine.width / 2) + i, sizeImage.height); // point on bottom
		cv::line(mask, p1, p2, 255, 1, 8);
	}

	//fill horizontal lines:
	int heightPoint = -tan(angleRadians) * sizeImage.width / 2;
	int sign = Algorithms::sign(heightPoint);
	cv::Point p1_middle = pCenter + sign *  (cv::Point(sizeImage.width / 2, 0) + cv::Point(0, heightPoint));
	cv::Point p2_middle = pCenter - sign * (cv::Point(sizeImage.width / 2, 0) + cv::Point(0, heightPoint));
	if (p1_middle.x > p2_middle.x)
	{
		std::swap(p1_middle, p2_middle);
	}

	assert(!Algorithms::numbersEqual(cos(angleRadians), 0));
	int count = static_cast<int>( ( sizeLine.height / fabs(cos(angleRadians))));
	for(int k = 0; k < count; k++)
	{
		cv::Point p1 = p1_middle + cv::Point(0, -count / 2 + k);
		cv::Point p2 = p2_middle + cv::Point(0, -count / 2 + k);
		cv::line(mask, p1, p2, 255, 1, 8);
	}

	// clear extra lines if needed (for non-central elements):
	Parall_m parallVertical;
	parallVertical.tl = cv::Point(pCenter.x - (sizeLine.width / 2), 0);
	parallVertical.tr = cv::Point(pCenter.x + (sizeLine.width / 2), 0);
	parallVertical.bl = cv::Point(pCenter.x - (sizeLine.width / 2), sizeImage.height);
	parallVertical.br = cv::Point(pCenter.x + (sizeLine.width / 2), sizeImage.height);

	Parall_m parallSloped;
	parallSloped.tl = p1_middle + cv::Point(0, -count / 2);
	parallSloped.tr = p2_middle + cv::Point(sizeImage.width, -count / 2);
	parallSloped.bl = p1_middle + cv::Point(0, count / 2);
	parallSloped.br = p2_middle + cv::Point(sizeImage.width, count / 2);

	clearExtraLines(parallVertical, parallSloped, nodeType, mask);
}


void AlgorithmsImages::clearExtraLines(const Parall_m& parallVertical, const Parall_m& parallHorizontal, NodeType nodeType, cv::Mat& mask)
{
	switch (nodeType)
	{
	case CENTER:
		break;
	case TOP_LEFT:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_TOP, mask);
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_LEFT, mask);
		break;
	case TOP_RIGHT:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_TOP, mask);
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_RIGHT, mask);
		break;
	case BOTTOM_LEFT:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_BOTTOM, mask);
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_LEFT, mask);
		break;
	case BOTTOM_RIGHT:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_BOTTOM, mask);
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_RIGHT, mask);
		break;
	case LEFT:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_LEFT, mask);
		break;
	case TOP:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_TOP, mask);
		break;
	case RIGHT:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_RIGHT, mask);
		break;
	case BOTTOM:
		clearExtraLine(parallVertical, parallHorizontal, MASK_PART_BOTTOM, mask);
		break;
	default:
		break;
	}
}

void AlgorithmsImages::clearExtraLine(const Parall_m& parallVertical, const Parall_m& parallHorizontal, MaskPart maskPart, cv::Mat& mask)
{
	switch (maskPart)
	{
		int height;
	case MASK_PART_TOP:
		height = std::max(parallHorizontal.tl.y, parallHorizontal.tr.y);
		for (int i = 0; i < height; i++)
		{
			cv::line(mask, parallHorizontal.tl + cv::Point(0, -i), parallHorizontal.tr + cv::Point(0, -i), 0, 1, 8);
		}
		break;
	case MASK_PART_BOTTOM:
		height = std::min(parallHorizontal.bl.y, parallHorizontal.br.y);
		for (int i = 0; i < height; i++)
		{
			cv::line(mask, parallHorizontal.bl + cv::Point(0, i), parallHorizontal.br + cv::Point(0, i), 0, 1, 8);
		}
		break;
	case MASK_PART_LEFT:
		for (int col = 0; col < parallVertical.tl.x; col++)
		{
			cv::line(mask, cv::Point(col, 0), cv::Point(col, mask.rows - 1), 0, 1, 8);
		}
		break;
	case MASK_PART_RIGHT:
		for (int col = parallVertical.tr.x; col < mask.cols; col++)
		{
			cv::line(mask, cv::Point(col, 0), cv::Point(col, mask.rows - 1), 0, 1, 8);
		}
		break;
	default:
		break;
	}
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

void AlgorithmsImages::downsample(const cv::Mat& cvImage, cv::Mat& cvImageResult)
{
	cv::Mat cvImageDownsampled;
	cv::pyrDown(cvImage, cvImageDownsampled, cv::Size(cvImage.cols / 2, cvImage.rows / 2));
	cv::pyrUp(cvImageDownsampled, cvImageResult, cv::Size(cvImage.cols , cvImage.rows ));
}


void AlgorithmsImages::getLineThickness(const std::vector<double>& sums, int& thickness)
{
	std::vector<double> sumsSmooth;

	cv::GaussianBlur(sums, sumsSmooth, cv::Size(11, 1), 0, 0, cv::BORDER_REPLICATE);

	std::vector<double> derivatives;
	Algorithms::differentiate(sumsSmooth, derivatives);
	Algorithms::nullifyBounds(derivatives.size() / 10, derivatives);
	DataTransfer::saveValuesToFile(sumsSmooth, "sums_smooth.txt");
	DataTransfer::saveValuesToFile(derivatives, "derivatives.txt");

	std::vector<int> peaks;
	Algorithms::findPeaks(derivatives, 2, 5, peaks);
	thickness = peaks[1] - peaks[0];
}

void AlgorithmsImages::findCrosshair(const cv::Mat& cvSubImage, const cv::Point& posImage, cv::Point& crosshair)
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