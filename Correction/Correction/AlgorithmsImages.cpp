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

void AlgorithmsImages::sumIntensityVertically(const cv::Mat& cvImage, std::vector<double>& sums)
{
	assert(!cvImage.empty());
	if (cvImage.empty())
	{
		return;
	}
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
	if (cvImage.empty())
	{
		return;
	}
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

void AlgorithmsImages::sumIntensityVerticallyWithSlope(const cv::Mat& cvImage, double angleRadians, std::vector<double>& sums)
{
	const int c_count = cvImage.rows + cvImage.cols;
	sums.resize(c_count);
	for (int i = 0; i < c_count; i++)
	{
		// find 2 points, between which we will draw line:
		int rho = -i;
		double theta = - angleRadians;
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
	case NODETYPE_CENTER:
		break;
	case NODETYPE_TOP_LEFT:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_TOP, mask);
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_LEFT, mask);
		break;
	case NODETYPE_TOP_RIGHT:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_TOP, mask);
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_RIGHT, mask);
		break;
	case NODETYPE_BOTTOM_LEFT:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_BOTTOM, mask);
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_LEFT, mask);
		break;
	case NODETYPE_BOTTOM_RIGHT:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_BOTTOM, mask);
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_RIGHT, mask);
		break;
	case NODETYPE_LEFT:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_LEFT, mask);
		break;
	case NODETYPE_TOP:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_TOP, mask);
		break;
	case NODETYPE_RIGHT:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_RIGHT, mask);
		break;
	case NODETYPE_BOTTOM:
		clearMaskPart(parallVertical, parallHorizontal, MASK_PART_BOTTOM, mask);
		break;
	default:
		break;
	}
}

void AlgorithmsImages::clearMaskPart(const Parall_m& parallVertical, const Parall_m& parallHorizontal, MaskPart maskPart, cv::Mat& mask)
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


void AlgorithmsImages::fillImageFragmentByTemplate(cv::Mat& image, const cv::Rect& ROI, const cv::Mat& imageTemplate)
{
	assert(!image.empty() && !imageTemplate.empty());
	if (image.empty() || imageTemplate.empty())
		return;

	//calculate number of subimages by vertical and horizontal:
	cv::Size sizeImageFragment(ROI.width, ROI.height);
	int blocksCountX = sizeImageFragment.width / imageTemplate.cols + 1;
	int blocksCountY = sizeImageFragment.height / imageTemplate.rows + 1;

	//for all images fill image:
#pragma omp parallel for
	for (int blockY = 0; blockY < blocksCountY; blockY++)
	{
		for (int blockX = 0; blockX < blocksCountX; blockX++)
		{
			int blockWidth = imageTemplate.cols;
			int blockHeight = imageTemplate.rows;
			cv::Rect rectBlock = cv::Rect(ROI.x + blockX * blockWidth, ROI.y + blockY * blockHeight, blockWidth, blockHeight);
			rectBlock = rectBlock & ROI;
			imageTemplate(cv::Rect(0, 0, std::min(blockWidth, rectBlock.width), std::min(blockHeight,rectBlock.height))).copyTo(image(rectBlock));
		}
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

void AlgorithmsImages::downsample(const cv::Mat& cvImage, cv::Mat& cvImageResult)
{
	cv::Mat cvImageDownsampled;
	cv::pyrDown(cvImage, cvImageDownsampled, cv::Size(cvImage.cols / 2, cvImage.rows / 2));
	cv::pyrUp(cvImageDownsampled, cvImageResult, cv::Size(cvImage.cols , cvImage.rows ));
}


void AlgorithmsImages::getLineThickness(const std::vector<double>& sums, int peakNeighborhood, int blur, int& thickness)
{
	std::vector<double> sumsSmooth;

	if (blur > 0)
	{
		if (blur % 2 == 0)
		{
			blur -= 1;
		}
		cv::GaussianBlur(sums, sumsSmooth, cv::Size(blur, 1), 0, 0, cv::BORDER_REPLICATE);
	}

	std::vector<double> derivatives;
	Algorithms::differentiate(sumsSmooth, derivatives);
	Algorithms::nullifyBounds(derivatives.size() / 10, derivatives);

	std::vector<int> peaks;
	Algorithms::findPeaks(derivatives, 2, peakNeighborhood, peaks);
	thickness = peaks[1] - peaks[0];
}

void AlgorithmsImages::getNodeSubImageROI(const cv::Size& cvImageSize, const cv::Size& sizeROI, const cv::Point& node, cv::Rect& ROIcorrected)
{
	int left = std::max(0, node.x - sizeROI.width / 2);
	int right = std::min(cvImageSize.width - 1, node.x + sizeROI.width / 2);
	int top = std::max(0, node.y - sizeROI.height / 2);
	int bottom = std::min(cvImageSize.height - 1, node.y + sizeROI.height / 2);
	ROIcorrected = cv::Rect(cv::Point(left, top), cv::Point(right, bottom));
}

void AlgorithmsImages::getImageExtents(const cv::Mat& cvImage, const cv::Size& sizeROI, const cv::Point& node, int& leftExt, int& rightExt, int& bottomExt, int& topExt)
{
	leftExt = rightExt = topExt = bottomExt = 0;

	const int c_left = node.x - sizeROI.width / 2;
	const int c_left_border = 0;
	if (c_left < c_left_border)
	{
		leftExt = c_left_border - c_left;
	}

	const int c_right = node.x + sizeROI.width / 2;
	const int c_right_border = cvImage.cols - 1;
	if (c_right_border < c_right )
	{
		rightExt = c_right - c_right_border;
	}

	const int c_top = node.y - sizeROI.height / 2;
	const int c_top_border = 0;
	if (c_top < c_top_border)
	{
		topExt = c_top_border - c_top;
	}

	const int c_bottom_border = cvImage.rows - 1;
	const int c_bottom = node.y + sizeROI.height / 2;
	if (c_bottom_border < c_bottom)
	{
		bottomExt = c_bottom - c_bottom_border;
	}
}

void AlgorithmsImages::sumToSumByStrips(const std::vector<double>& sums, int widthStrip, std::vector<double>& sumByStrips)
{
	assert(widthStrip > 0);
	assert(sums.size() > widthStrip);
	if (sums.size() <= widthStrip || widthStrip <= 0)
	{
		return;
	}
	sumByStrips.resize(sums.size() - widthStrip);
	for (int i = 0; i < sumByStrips.size(); i++)
	{
		auto iterFirst = sums.begin() + i;
		auto iterLast = iterFirst + widthStrip;
		sumByStrips[i] = std::accumulate(iterFirst, iterLast, 0) / widthStrip;
	}
}