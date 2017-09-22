#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ImageController.h"
#include "AlgorithmsImages.h"
#include "ImageDisplay.h"

ImageController::ImageController(QObject* parent) : QObject(parent)
{

}

ImageController::~ImageController()
{

}

void ImageController::readImage(const std::string& filename)
{
	cv::Mat a = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE); // for testing
	std::swap(cvImage_, a);
}

void ImageController::createVisualImageBlocks(ImageDisplay& imageDisplay)
{
	Q_ASSERT(cvImage_.size > 0);
	const int c_block_width = 2000;
	const int c_block_height = 2000;
	AlgorithmsImages::createVisualImageBlocks(cvImage_,c_block_width, c_block_height, imageDisplay);
}

void ImageController::findNodesApproximately()
{
	AlgorithmsImages::findNodesApproximately(cvImage_, nodesSet_);
	AlgorithmsImages::clarifyNodes(cvImage_, nodesSet_);
	AlgorithmsImages::clarifyNodes2(cvImage_, nodesSet_);
}

void ImageController::findNodesAccurately()
{
	/*cv::Point nodeCentral = nodesSet_.center();
	cv::Size sizeImage(100, 100);
	cv::Mat cvSubImage = cv::Mat(cvImage_, cv::Rect(nodeCentral - cv::Point(sizeImage / 2), nodeCentral + cv::Point(sizeImage / 2)));
	cv::Size sizeLine;
	AlgorithmsImages::getLineSize(cvSubImage, sizeLine);*/

	cv::Size sizeImage = nodesSet_.getCellSize() / 2;

	for (int row = 0; row < nodesSet_.rows(); row++)
	{
		for (int col = 0; col < nodesSet_.cols(); col++)
		{
			cv::Point& node = nodesSet_.at(row, col);
			cv::Rect roiCorrected;
			AlgorithmsImages::getNodeSubImageROI(cvImage_, sizeImage, node, roiCorrected);
			cv::Size sizeLine;
			cv::Mat cvSubImage = cv::Mat(cvImage_, roiCorrected);
			AlgorithmsImages::getLineSize(cvSubImage, sizeLine);
			cv::Mat mask;
			NodeType nodeType = nodesSet_.getNodeType(row, col);
			AlgorithmsImages::generateMask(sizeLine, nodeType, mask);
			cv::Mat corrMatrix;
			cv::matchTemplate(cvSubImage, mask, corrMatrix, cv::TM_CCORR_NORMED);
			double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
			cv::minMaxLoc(corrMatrix, &minVal, &maxVal, &minLoc, &maxLoc);
			cv::Point nodeNew = roiCorrected.tl() + maxLoc + cv::Point(mask.cols/2, mask.rows/2);
			node = nodeNew;
		}
	}
}

QVector<QPoint> ImageController::getNodesVisual() const
{
	QVector<QPoint> nodesPositions(nodesSet_.rows() * nodesSet_.cols());
	for (int i = 0; i < nodesPositions.size(); i++)
	{
		int col = i % nodesSet_.cols();
		int row = i / nodesSet_.rows();
		cv::Point p(nodesSet_.at(row, col));
		nodesPositions[i] = QPoint(p.x, p.y);
	}
	return nodesPositions;
}

