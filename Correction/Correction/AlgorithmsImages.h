#pragma once

#include <opencv2/core/core.hpp>

#include <QImage>

#include "NodesSet.h"

struct ImageDisplay;

namespace AlgorithmsImages
{
	QImage Mat2QImageGray(const cv::Mat_<uchar> &src);
	void getLineSize(const cv::Mat& cvImage, cv::Size& size);
	void findCrosshair(const cv::Mat& cvSubImage, const cv::Point& posImage, cv::Point& crosshair);
	void createVisualImageBlocks(const cv::Mat& cvImage, int blockWidth, int blockHeight, ImageDisplay& imageDisplay);
	void findNodesApproximately(const cv::Mat& cvImage, NodesSet& nodesSet);
	void clarifyNodes(const cv::Mat& cvImage, NodesSet& nodesSet);
	void clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet);
	void sumIntensityVertically(const cv::Mat& cvImage, std::vector<double>& sums);
	void sumIntensityHorizontally(const cv::Mat& cvImage, std::vector<double>& sums);
	void getNodeSubImageROI(const cv::Mat& cvImage, const cv::Size& sizeROI, const cv::Point& node, cv::Rect& ROIcorrected);
	void generateMask(const cv::Size& sizeLine, NodeType nodeType, cv::Mat& mask);
}
