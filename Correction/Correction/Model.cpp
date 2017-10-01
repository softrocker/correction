#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Model.h"
#include "AlgorithmsImages.h"
#include "ImageDisplay.h"
#include "AlgorithmsCalculus.h"
#include "DataTransfer.h"
#include "Parameters.h"

#include <QMessageBox>
#include <QVariantList>

#define _USE_MATH_DEFINES // for C++  
#include <math.h>  

cv::Matx<double, 3, 3> getScansTrans(const NodesSet& nodesSet, const cv::Point& origin);


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

	clarifyNodes0(cvImage_, nodesSet_, rows, cols);
	clarifyNodes1(cvImage_, nodesSet_);
	clarifyNodes2(cvImage_, nodesSet_);
}

void Model::clarifyNodes0(const cv::Mat& cvImage, NodesSet& nodesSet, int grid_rows, int grid_cols)
{
	assert(!cvImage.empty());
	if (cvImage.empty())
	{
		return;
	}
	//array of values sums of f(x,y) by y:
	std::vector<double> sumsAlongY(cvImage.cols);
	AlgorithmsImages::sumIntensityVertically(cvImage, sumsAlongY);

	//array of values sums of f(x,y) by X:
	std::vector<double> sumsAlongX(cvImage.rows);
	AlgorithmsImages::sumIntensityHorizontally(cvImage, sumsAlongX);

	//finding peaks:
	std::vector<int> peaksY;
	Algorithms::findPeaks(sumsAlongX, grid_rows, c_neighborhood, peaksY);
	std::vector<int> peaksX;
	Algorithms::findPeaks(sumsAlongY, grid_cols, c_neighborhood, peaksX);

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

	nodesSet.create(nodes, grid_cols, grid_rows);
}

void Model::clarifyNodes1(const cv::Mat& cvImage, NodesSet& nodesSet)
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
		AlgorithmsImages::sumIntensityHorizontally(cvSubImage, sums);

		//find peaks:
		std::vector<int> peaks;
		Algorithms::findPeaks(sums, nodesSet.rows(), c_neighborhood, peaks);
		std::reverse(peaks.begin(), peaks.end());

		// shift each node in column to corresponding peak
		for (int row = 0; row < nodesSet.rows(); row++)
		{
			cv::Point pNew = cv::Point(nodesSet.at(row, col).x, peaks[row]);
			cv::Point pOld = nodesSet.at(row, col);
			nodesSet.setNode(row, col, pNew);
		}

		int progressInPercents = 100 * col / nodesSet_.cols();
		emit sendProgressS(progressInPercents);
	}
	emit operationfinishedS();
}

void Model::clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet)
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
			AlgorithmsImages::getNodeSubImageROI(cvImage, sizeSubImage, node, roiSubImage);
			cv::Mat cvSubImage = cv::Mat(cvImage, cv::Rect(roiSubImage.tl(), roiSubImage.br()));

			//integrate subImage along Y-axis:
			std::vector<double> sums;
			AlgorithmsImages::sumIntensityVertically(cvSubImage, sums);

			//find single peak:
			std::vector<int> peaks;
			Algorithms::findPeaks(sums, 1, 0, peaks);

			//shift node to finded peak:
			int nodeXLocal = node.x - roiSubImage.tl().x;
			cv::Point pNew = nodesSet.at(row, col) + cv::Point(peaks[0] - nodeXLocal, 0);
			nodesSet.setNode(row, col, pNew);

			int nodesCount = nodesSet_.rows() * nodesSet_.cols();
			int progressInPercents = 100 * (row * nodesSet_.cols() + col) / nodesCount;
			emit sendProgressS(progressInPercents);
		}
	}
	emit operationfinishedS();
	emit updateVisualizationS();
}

void Model::findNodesAccurately(int rows, int cols)
{
	if (!valid())
	{
		return;
	}
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

			double angle = nodesSet_.getAngle(row, col, -1);

			//find line thickness for sloped horizontal line:
			std::vector<double> sumsSloped;
			AlgorithmsImages::sumIntensityHorizontallyWithSlope(cvSubImage, angle, sumsSloped);
			int thicknessLineSloped = -1;
			AlgorithmsImages::getLineThickness(sumsSloped, thicknessLineSloped);

			//find line thickness for vertical line:
			std::vector<double> sumsX;
			AlgorithmsImages::sumIntensityVertically(cvSubImage, sumsX);
			int thicknessLineVertical = -1;
			AlgorithmsImages::getLineThickness(sumsX, thicknessLineVertical);

			//create mask with angle:
			NodeType nodeType = nodesSet_.getNodeType(row, col);
			cv::Mat mask;
			AlgorithmsImages::generateMaskWithAngle(sizeImage / 2, cv::Size(thicknessLineVertical, thicknessLineSloped), angle, nodeType, mask);
			cv::Mat corrMatrix;

			cv::matchTemplate(cvSubImage, mask, corrMatrix, cv::TM_CCORR_NORMED);
			double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
			cv::minMaxLoc(corrMatrix, &minVal, &maxVal, &minLoc, &maxLoc);
			cv::Point nodeNew = roiCorrected.tl() + maxLoc + cv::Point(mask.cols / 2, mask.rows / 2);
			node = nodeNew;

			int nodesCount = nodesSet_.rows() * nodesSet_.cols();
			int progressInPercents = 100 * (row * nodesSet_.cols() + col) / nodesCount;
			emit sendProgressS(progressInPercents);
		}
	}
	emit operationfinishedS();
	emit updateVisualizationS();
}

void Model::calculateCorrectionTable(std::vector<double>& correctionTable)
{
	assert(!nodesSet_.empty());
	/* calculate parameters of ideal grid: */
	// calculate origin of ideal grid:
	const int c_cols = nodesSet_.cols();
	const int c_rows = nodesSet_.rows();
	cv::Point nodeTop = nodesSet_.at(c_rows - 2, c_cols / 2);
	cv::Point nodeBottom = nodesSet_.at(1, c_cols / 2);
	cv::Point nodeLeft = nodesSet_.at(c_rows / 2, 1);
	cv::Point nodeRight = nodesSet_.at(c_rows / 2, c_cols - 2);
	cv::Point originOfIdealGrid = cv::Point((nodeLeft.x + nodeRight.x) / 2, (nodeBottom.y + nodeTop.y) / 2);

	//calculate direction of main axis of grid:
	cv::Vec4f lineOx;

	std::vector<cv::Point> valuesX = nodesSet_.row(c_rows / 2);
	valuesX.erase(valuesX.end() - 1); // remove first and last elements because their position are too much distorted.
	valuesX.erase(valuesX.begin());
	cv::fitLine(valuesX, lineOx, CV_DIST_L2, 0, 0.01, 0.01);

	cv::Vec4f lineOy;
	std::vector<cv::Point> valuesY = nodesSet_.col(c_cols / 2);
	valuesY.erase(valuesY.end() - 1); // remove first and last elements because their position are too much distorted.
	valuesY.erase(valuesY.begin());
	cv::fitLine(valuesY, lineOy, CV_DIST_L2, 0, 0.01, 0.01);

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

	//find line thickness for vertical line:
	std::vector<double> sumsX;
	AlgorithmsImages::sumIntensityVertically(cvSubImage, sumsX);
	int thicknessLineVertical = -1;
	AlgorithmsImages::getLineThickness(sumsX, thicknessLineVertical);

	//find line thickness for sloped horizontal line:
	std::vector<double> sumsSloped;
	AlgorithmsImages::sumIntensityHorizontallyWithSlope(cvSubImage, angle, sumsSloped);
	int thicknessLineSloped = -1;
	AlgorithmsImages::getLineThickness(sumsSloped, thicknessLineSloped);

	//create mask with angle:
	NodeType nodeType = nodesSet_.getNodeType(row, col);
	cv::Mat mask;
	AlgorithmsImages::generateMaskWithAngle(sizeImage / 2, cv::Size(thicknessLineVertical, thicknessLineSloped), angle, nodeType, mask);
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

void Model::doOperation(const Operation& operation, const QVariantList& operationParameters)
{
	std::vector<double> correctionValues;
	int rows_count(-1);
	int cols_count(-1);

	switch (operation)
	{

	case OPERATION_FIND_NODES_APPROX:
		assert(operationParameters.size() == 2);
		if (operationParameters.size() != 2)
		{
			return;
		}
		rows_count = operationParameters[0].toInt();
		cols_count = operationParameters[1].toInt();
		findNodesApproximately(rows_count, cols_count);
		break;

	case OPERATION_FIND_NODES_ACCURATE:
		assert(operationParameters.size() == 2);
		if (operationParameters.size() != 2)
		{
			return;
		}
		rows_count = operationParameters[0].toInt();
		cols_count = operationParameters[1].toInt();
		findNodesAccurately(rows_count, cols_count);
		break;

	case OPERATION_WRITE_CORRECTION_TABLE:

		calculateCorrectionTable(correctionValues);
		break;

	default:
		break;
	}
}

void Model::getRowColByIndex(int index, int& row, int& col)
{
	row = index / nodesSet_.cols();
	col = index % nodesSet_.cols();
}

cv::Matx<double, 3, 3> getScansTrans(const NodesSet& nodesSet, const cv::Point& origin)
{
	cv::Matx<double, 3, 3> mx;

	cv::Vec3d rx(0, 0, 0);
	cv::Vec3d ry(0, 0, 0);

	const int c_cols_count = nodesSet.cols();
	const int c_rows_count = nodesSet.rows();

	double ds = 0x7fffffff;

	ds /= (c_cols_count / 2);

	int ox = c_cols_count / 2, oy = c_rows_count / 2;

	for (int row = 0; row < c_rows_count; ++row)
	{
		for (int col = 0; col < c_cols_count; ++col)
		{
			int k, l;
			cv::Vec2d ov(cv::Point2d(nodesSet.at(row, col)) - cv::Point2d(origin));

			double x, y;
			x = (col - ox) * ds;
			y = (row - oy) * ds;

			double x2, y2, xy;
			x2 = x * x, y2 = y * y, xy = x * y;

			rx[0] += ov[0] * x, rx[1] += ov[0] * y/*, rx.z += ov.x * xy*/;
			ry[0] += ov[1] * x, ry[1] += ov[1] * y/*, ry.z += ov.y * xy*/;

			mx(0, 0) += x2;
			mx(0, 1) += xy;
			mx(1, 1) += y2;
		}
	}
	mx(1, 0) = mx(0, 1);//[0][1];
	mx(2, 0) = mx(0, 2);// mx[2][0] = mx[0][2],
	mx(2, 1) = mx(1, 2);// mx[2][1] = mx[1][2];
	mx(2, 2) = 1.0; // mx[2][2] = 1.0;

	if (cv::invert(mx, mx))
	{
		rx = mx * rx;
		ry = mx * ry;
	}

	mx(0, 0) = rx[0], mx(0, 1) = rx[1], mx(0, 2) = origin.x; //mx[0][0] = rx.x, mx[0][1] = rx.y, mx[0][2] = or .x;
	mx(1, 0) = ry[0], mx(1, 1) = ry[1], mx(1, 2) = origin.y;//mx[1][0] = ry.x, mx[1][1] = ry.y, mx[1][2] = or .y;
	mx(2, 0) = 0, mx(2, 1) = 0, mx(2, 2) = 1; //mx[2][0] = 0, mx[2][1] = 0, mx[2][2] = 1;

	return mx;
}