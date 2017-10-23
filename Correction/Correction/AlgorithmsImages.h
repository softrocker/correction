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
	QImage Mat2QImageGray(const cv::Mat_<uchar> &src);
	void downsample(const cv::Mat& cvImage, cv::Mat& cvImageResult);
	void getLineThickness(const std::vector<double>& sums, int peakNeighborhood, int blur, int& thickness);
	void findCrosshair(const cv::Mat& cvSubImage, const cv::Point& posImage, cv::Point& crosshair);
	void createVisualImageBlocks(const cv::Mat& cvImage, int blockWidth, int blockHeight, ImageDisplay& imageDisplay);

	void sumIntensityVertically(const cv::Mat& cvImage, std::vector<double>& sums);
	void sumIntensityHorizontally(const cv::Mat& cvImage, std::vector<double>& sums);
	void sumIntensityHorizontallyWithSlope(const cv::Mat& cvImage, double angleRadians, std::vector<double>& sums);
	void getNodeSubImageROI(const cv::Mat& cvImage, const cv::Size& sizeROI, const cv::Point& node, cv::Rect& ROIcorrected);
	void getImageExtents(const cv::Mat& cvImage, const cv::Size& sizeROI, const cv::Point& node, int& left, int& rigth, int& bottom, int& top);
	void expandImage(NodeType nodeType, cv::Mat& cvImageDst);
	void sumToSumByStrips(const std::vector<double>& sums, int widthStrip,  std::vector<double>& sumByStrips);
	void generateMask(const cv::Size& sizeImage, const cv::Size& sizeLine, NodeType nodeType, cv::Mat& mask);

	//algorithms that allows to generate mask considering angle of horizontal line:
	void generateMaskWithAngle(const cv::Size& sizeImage, const cv::Size& sizeLine, double angleRadians, NodeType nodeType, cv::Mat& mask);
	void clearExtraLines(const Parall_m& parallVertical, const Parall_m& parallHorizontal, NodeType nodeType, cv::Mat& mask);
	void clearExtraLine(const Parall_m& parallVertical, const Parall_m& parallHorizontal, MaskPart maskPart, cv::Mat& mask);

}
