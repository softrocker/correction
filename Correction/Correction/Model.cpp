#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Model.h"
#include "AlgorithmsImages.h"
#include "ImageDisplay.h"

Model::Model(QObject* parent) : QObject(parent)
{

}

Model::~Model()
{

}

void Model::setImage(const cv::Mat& cvImage)
{
	cvImage_ = cvImage;
}

//void Model::readImage(const std::string& filename)
//{
//	cv::Mat a = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE); // for testing
//	std::swap(cvImage_, a);
//}



void Model::findNodesApproximately()
{
	AlgorithmsImages::findNodesApproximately(cvImage_, nodesSet_);
	AlgorithmsImages::clarifyNodes(cvImage_, nodesSet_);
	AlgorithmsImages::clarifyNodes2(cvImage_, nodesSet_);
}

void Model::findNodesAccurately()
{
	cv::Size sizeImage = nodesSet_.getCellSize();

	for (int row = 0; row < nodesSet_.rows(); row++)
	{
		for (int col = 0; col < nodesSet_.cols(); col++)
		{
			cv::Point& node = nodesSet_.at(row, col);
			cv::Rect roiCorrected;
			AlgorithmsImages::getNodeSubImageROI(cvImage_, sizeImage, node, roiCorrected);
			cv::Size sizeLine;
			cv::Mat cvSubImage = cv::Mat(cvImage_, roiCorrected);

			const int c_iteration_count = 4;
			for (int i = 0; i < c_iteration_count; i++)
			{
				AlgorithmsImages::downsample(cvSubImage, cvSubImage);
			}

			AlgorithmsImages::getLineSize(cvSubImage, sizeLine);
			
			cv::Mat mask;
			NodeType nodeType = nodesSet_.getNodeType(row, col);

			AlgorithmsImages::generateMask(sizeImage / 3, sizeLine, nodeType, mask);
			cv::Mat corrMatrix;

			cv::matchTemplate(cvSubImage, mask, corrMatrix, cv::TM_CCORR_NORMED);
			double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
			cv::minMaxLoc(corrMatrix, &minVal, &maxVal, &minLoc, &maxLoc);
			cv::Point nodeNew = roiCorrected.tl() + maxLoc + cv::Point(mask.cols/2, mask.rows/2);
			node = nodeNew;
		}
	}
}

QVector<QPoint> Model::getNodesVisual() const
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

void Model::test()
{
	cv::Size sizeImage = nodesSet_.getCellSize() / 2;
	cv::Point node = nodesSet_.center();
	cv::Rect roiCorrected;
	AlgorithmsImages::getNodeSubImageROI(cvImage_, sizeImage, node, roiCorrected);
	cv::Mat cvSubImage = cv::Mat(cvImage_, roiCorrected);

	cv::Mat cvSubImageBinarized;
	cv::threshold(cvSubImage, cvSubImageBinarized, 120, 255, cv::THRESH_BINARY);

	std::vector<cv::Vec2f>lines;
	HoughLines(cvSubImageBinarized, lines, 1, CV_PI / 180, std::min(roiCorrected.width, roiCorrected.height));

	cv::Mat imageWithLinesColor;
	cv::cvtColor(cvSubImageBinarized, imageWithLinesColor, cv::COLOR_GRAY2BGR);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		cv::Point pt1(cvRound(x0 + 1000 * (-b)),
			cvRound(y0 + 1000 * (a)));
		cv::Point pt2(cvRound(x0 - 1000 * (-b)),
			cvRound(y0 - 1000 * (a)));
		cv::line(imageWithLinesColor, pt1, pt2, cv::Scalar(0, 0, 255), 3, 8);
	}
	//cv::Mat cvSubImageDownsampled = cvSubImage.clone();
	//const int c_iteration_count = 4;
	//for (int i = 0; i < c_iteration_count; i++)
	//{
	//	AlgorithmsImages::downsample(cvSubImageDownsampled, cvSubImageDownsampled);
	//}
}