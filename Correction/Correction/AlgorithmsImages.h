#pragma once

#include <opencv2/core/core.hpp>

#include <QImage>

#include "NodesSet.h"

struct ImageDisplay;

enum MaskPart
{
	MASK_PART_TOP = 0,
	MASK_PART_BOTTOM = 1,
	MASK_PART_LEFT = 2,
	MASK_PART_RIGHT = 3
};

enum SummationDirection
{
	SUM_VERTICALLY = 0,
	SUM_HORIZONTALLY = 1
};

//parallelogramm
struct Parall_m 
{
	cv::Point tl;
	cv::Point bl;
	cv::Point tr;
	cv::Point br;
};

namespace AlgorithmsImages
{
	/* Function returns grayscale Qt-image by given opencv-image */
	QImage Mat2QImageGray(const cv::Mat_<uchar> &src);

	/*
	Downsample current image (cvImage). 
	Resulted image cvImageResult is image with size (cvImage.width / 2 x  cvImage.height / 2)
	*/
	void downsample(const cv::Mat& cvImage, cv::Mat& cvImageResult);

	/*
	Procedure calculates thickness of peak of input numerical values vector (thickness of line in our application)
	parameters:
	sums - input vector,
	peakNeighborhood - how many values will be removed to the left and to the right from peak
	blur - smoothing window size
	thickness - thickness of peak (thickness of line)
	*/
	void getLineThickness(const std::vector<double>& sums, int peakNeighborhood, int blur, int& thickness);

	/* 
	Procedure calculates collection of Qt-images for visualization
	parameters:
	cvImage - input opencv image
	blockWidth - Qt-image block width
	blockHeight - Qt-image block height
	imageDisplay - collection of Qt-images and corresponding parameters
	*/
	void createVisualImageBlocks(const cv::Mat& cvImage, int blockWidth, int blockHeight, ImageDisplay& imageDisplay);

	/*
	Procedure sums up intensity of input image (cvImage) vertically and puts corresponding values to vector (sums) 
	*/
	void sumIntensityVertically(const cv::Mat& cvImage, std::vector<double>& sums);

	/*
	Procedure sums up intensity of input image (cvImage) horizontally and puts corresponding values to vector (sums)
	*/
	void sumIntensityHorizontally(const cv::Mat& cvImage, std::vector<double>& sums);

	/*
	Procedure sums up intensity of input image (cvImage) "pseudo-horizontally",
	considering angle between "pseudo-horizontal" line and Ox axis (angleRadians),
	puts corresponding values to vector (sums)
	*/
	void sumIntensityHorizontallyWithSlope(const cv::Mat& cvImage, double angleRadians, std::vector<double>& sums);

	void sumIntensityVerticallyWithSlope(const cv::Mat& cvImage, double angleRadians, std::vector<double>& sums);
	/*
	Procedure calculates region of interest for given image.
	parameters:
	cvImage - input initial big image
	sizeROI - size of region of interest
	node - center point of region of interest (m.b. rename node for something else?)
	ROIcorrected - sought-for region of interest (considering initial size restrictions)
	*/
	void getNodeSubImageROI(const cv::Size& cvImage, const cv::Size& sizeROI, const cv::Point& node, cv::Rect& ROIcorrected);

	/*
	Procedure calculates extents of given image 
	(how much image with size 'sizeROI' with the center in 'node' will be cropped from the left, right, top and bottom
	to remain inside of 'cvImage').
	*/
	void getImageExtents(const cv::Mat& cvImage, const cv::Size& sizeROI, const cv::Point& node, int& left, int& rigth, int& bottom, int& top);

	/* 
	Procedure averages every 'widthStrip' consequent elements and put them to 
	'sumByStrips'
	*/
	void sumToSumByStrips(const std::vector<double>& sums, int widthStrip,  std::vector<double>& sumByStrips);

	/*
	Procedure generates mask, considering angle of "pseudo-horizontal" line
	parameters:
	sizeImage - size of mask
	sizeLine - parameter contains thickness of vertical line and thickness of "pseudo-horizontal" line
	angleRadians - angle between "pseudo-horizontal" line and Ox axis
	nodeType - type of node of grid (center, top, bottom, top-left etc.)
	*/ 
	void generateMaskWithAngle(const cv::Size& sizeImage, const cv::Size& sizeLine, double angleRadians, NodeType nodeType, cv::Mat& mask);

	/* Procedure that removes extra lines on mask, depending on node type 
	(for nodes of central type it's not needed to remove any lines) 
	parameters:
	parallVertical - parallelogramm (for our application version it's rectangle), corresponding to 
	vertical line of mask, considering line thickness.
	parallHorizontal - parallelogramm, corresponding to
	horizontal line of mask, considering line thickness.
	nodeType - type of node of grid (center, top, bottom, top-left etc.)
	mask - result of calculation (mask itself)
	*/
	void clearExtraLines(const Parall_m& parallVertical, const Parall_m& parallHorizontal, NodeType nodeType, cv::Mat& mask);

	/* Procedure that removes part of mask, defined by 'maskPart' parameter */
	void clearMaskPart(const Parall_m& parallVertical, const Parall_m& parallHorizontal, MaskPart maskPart, cv::Mat& mask);

	void fillImageFragmentByTemplate(cv::Mat& image, const cv::Rect& ROI, const cv::Mat& imageTemplate);
}
