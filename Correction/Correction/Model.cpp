#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Model.h"
#include "AlgorithmsImages.h"
#include "ImageDisplay.h"
#include "AlgorithmsCalculus.h"
#include "DataTransfer.h"

#include <QMessageBox>

#define _USE_MATH_DEFINES // for C++  
#include <math.h>  




Model::Model(QObject* parent) : QObject(parent)
{
	nodeCur_ = cv::Point(INT_MIN, INT_MIN);
	indexCur_ = INT_MIN;
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



void Model::findNodesApproximately(int rows, int cols)
{
	assert(!cvImage_.empty());
	if (cvImage_.empty())
	{
		return;
	}
	AlgorithmsImages::findNodesApproximately(cvImage_, nodesSet_,  rows,  cols);
	AlgorithmsImages::clarifyNodes(cvImage_, nodesSet_);
	AlgorithmsImages::clarifyNodes2(cvImage_, nodesSet_);
}

void Model::findNodesAccurately(int rows, int cols)
{
	if (!valid())
	{
		return;
	}
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
	assert(nodeCur_ != cv::Point(INT_MIN, INT_MIN));
	assert(indexCur_ != INT_MIN);
	if (nodeCur_ == cv::Point(INT_MIN, INT_MIN) || indexCur_ == INT_MIN)
	{
		return;
	}
	cv::Size sizeImage = nodesSet_.getCellSize() / 2;
	cv::Point node = nodeCur_;
	cv::Rect roiCorrected;
	AlgorithmsImages::getNodeSubImageROI(cvImage_, sizeImage, node, roiCorrected);
	cv::Mat cvSubImage = cv::Mat(cvImage_, roiCorrected);

	int row(-1);
	int col(-1);
	getRowColByIndex(indexCur_, row, col);
	double angle = nodesSet_.getAngle(row, col, -1);

	cv::Mat subImageColored;
	cv::cvtColor(cvSubImage, subImageColored, cv::COLOR_GRAY2BGR);
	//calculate 
	const int c_count = cvSubImage.rows + cvSubImage.cols;
	std::vector<double> sums(c_count);
	for (int i = 0; i < c_count; i++)
	{
		// find 2 points, between which we will draw line:
		int rho = -i;
		double theta = M_PI / 2 - angle;
		double a = cos(theta);
		double b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
			cv::Point pt1(cvRound(x0 + 1000 * (-b)),
				cvRound(y0 + 1000 * (a)));
			cv::Point pt2(cvRound(x0 - 1000 * (-b)),
				cvRound(y0 - 1000 * (a)));

		// sum points intensity along line:
		cv::LineIterator it(cvSubImage, pt1, pt2, 8);
		for (int k = 0; k < it.count; k++, ++it)
		{
			int value = cvSubImage.at<uchar>(it.pos());
			sums[i] += value;
		}
		sums[i] /= it.count;
	}
	
	DataTransfer::saveValuesToFile(sums, "sums.txt");

	std::vector<double> sumsSmooth;
	try
	{
		cv::GaussianBlur(sums, sumsSmooth, cv::Size(5, 1), 0, 0, cv::BORDER_REFLECT_101);
	}
	catch (std::exception& exc)
	{
		QMessageBox::critical(NULL, tr("Error during calculation"), QString(exc.what()));
	}

	DataTransfer::saveValuesToFile(sumsSmooth, "sums_smooth.txt");

	std::vector<double> derivatives;
	Algorithms::differentiate(sumsSmooth, derivatives);
	
	std::vector<int> peaks;
	Algorithms::findPeaks(derivatives, 2, 5, peaks);
	DataTransfer::saveValuesToFile(derivatives, "derivatives.txt");
	double lineWidth = peaks[1] - peaks[0];
}

void Model::setNodePosition(int index, int posX, int posY)
{
	int row = index / nodesSet_.cols();
	int col = index % nodesSet_.cols();
	cv::Point node = cv::Point(posX, posY);
	nodesSet_.setNode(row, col, node);
	nodeCur_ = node;
	indexCur_ = index;
}

bool Model::valid()
{
	assert(!cvImage_.empty());
	assert(!nodesSet_.empty());
	return (!cvImage_.empty() && !nodesSet_.empty());
}

void Model::getRowColByIndex(int index, int& row, int& col)
{
	row = index / nodesSet_.cols();
	col = index % nodesSet_.cols();
}
