#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Model.h"
#include "AlgorithmsImages.h"
#include "ImageDisplay.h"
#include "AlgorithmsCalculus.h"
#include "DataTransfer.h"
#include "Parameters.h"
#include <fstream>

#include <QMessageBox>
#include <QVariantList>

#define _USE_MATH_DEFINES // for C++  
#include <math.h>  

cv::Matx<double, 3, 3> getScansTrans(const NodesSet& nodesSet, const cv::Point& origin);

Model::Model(QObject* parent) : QObject(parent)
{
	//nodeCur_ = cv::Point(INT_MIN, INT_MIN);
	indexCur_ = INT_MIN;
}

Model::~Model()
{

}

void Model::setImage(const cv::Mat& cvImage)
{
	cvImage_ = cvImage;
}

const cv::Mat& Model::getImage()
{
	return cvImage_;
}

void Model::calculateBlocksToUpdate(const QVector<QRect>& imageBlockRects, std::vector<cv::Mat>& imageBlocks) const
{
	imageBlocks.reserve(imageBlockRects.size());
	for (int i = 0; i < imageBlockRects.size(); i++)
	{
		cv::Rect rect(imageBlockRects[i].x(), imageBlockRects[i].y(), imageBlockRects[i].width(), imageBlockRects[i].height());
		imageBlocks.push_back(cv::Mat(cvImage_, rect));
	}
}

//void Model::calculateBlocksToUpdate(const QRect& rectUpdate, int blockSizeX, int blockSizeY, std::vector<int>& indexesOfBlocks, std::vector<cv::Mat&>& imageBlocks)
//{
//
//}

void Model::findNodesApproximately(int rows, int cols, int peakNeighborhoodGlobal)
{
	assert(!cvImage_.empty());
	if (cvImage_.empty())
	{
		return;
	}
	problemRects_.clear();
	clarifyNodes0(cvImage_, nodesSet_, rows, cols, peakNeighborhoodGlobal);
	clarifyNodes1(cvImage_, nodesSet_, peakNeighborhoodGlobal);
	clarifyNodes2(cvImage_, nodesSet_);
	nodesSetBase_ = nodesSet_;
}

void Model::clarifyNodes0(const cv::Mat& cvImage, NodesSet& nodesSet, int grid_rows, int grid_cols, int peakNeighborhoodGlobal)
{

	assert(!cvImage.empty());
	if (cvImage.empty())
	{
		return;
	}
	//array of values sums of f(x,y) by y:
	std::vector<double> sumsAlongY(cvImage.cols);
	AlgorithmsImages::sumIntensityVertically(cvImage, sumsAlongY);
	DataTransfer::saveValuesToFile(sumsAlongY, "sums_Y.txt"); 

	//array of values sums of f(x,y) by X:
	std::vector<double> sumsAlongX(cvImage.rows);
	AlgorithmsImages::sumIntensityHorizontally(cvImage, sumsAlongX);
	DataTransfer::saveValuesToFile(sumsAlongX, "sums_X.txt");

	//finding peaks:
	std::vector<int> peaksY;
	Algorithms::findPeaks(sumsAlongX, grid_rows, peakNeighborhoodGlobal, peaksY);
	std::vector<int> peaksX;
	Algorithms::findPeaks(sumsAlongY, grid_cols, peakNeighborhoodGlobal, peaksX);

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

void Model::clarifyNodes1(const cv::Mat& cvImage, NodesSet& nodesSet, int peakNeighborhoodGlobal)
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
		Algorithms::findPeaks(sums, nodesSet.rows(), peakNeighborhoodGlobal, peaks);
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
			cv::Size sizeSubImage(cellSize.width, cellSize.height);
			cv::Rect roiSubImage;
			AlgorithmsImages::getNodeSubImageROI(cvImage.size(), sizeSubImage, node, roiSubImage);
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

void Model::findNodesAccurately(int rows, int cols, double cellSizeFactor, int blurImage, int blurMask, int peakNeighborhoodLocal)
{
	if (!valid())
	{
		return;
	}

	problemRects_.clear();

	for (int row = 0; row < nodesSet_.rows(); row++)
	{
		for (int col = 0; col < nodesSet_.cols(); col++)
		{
			findSingleNodeAccurately(row, col, cellSizeFactor, blurImage, blurMask, peakNeighborhoodLocal);

			int nodesCount = nodesSet_.rows() * nodesSet_.cols();
			int progressInPercents = 100 * (row * nodesSet_.cols() + col) / nodesCount;
			emit sendProgressS(progressInPercents);
		}
	}
	emit operationfinishedS();
	emit updateVisualizationS();
}

void Model::findSingleNodeAccurately(int row, int col, double cellSizeFactor, int blurImageSize, int blurMaskSize, int peakNeighborhoodLocal, bool nodeSelected)
{
	int row_local = row;
	int col_local = col;

	if (nodeSelected)
	{
		assert(indexCur_ != INT_MIN);
		if (indexCur_ == INT_MIN)
		{
			return;
		}
		getRowColByIndex(indexCur_, row_local, col_local);
	}

	cv::Point& node = nodesSet_.at(row_local, col_local);
	cv::Rect roiCorrected;
	cv::Size sizeCell = nodesSet_.getCellSize();
	cv::Size sizeSubImage = cv::Size(static_cast<int>(sizeCell.width * cellSizeFactor), static_cast<int>(sizeCell.height * cellSizeFactor));
	AlgorithmsImages::getNodeSubImageROI(cvImage_.size(), sizeSubImage, node, roiCorrected);
	cv::Mat cvSubImage = cv::Mat(cvImage_, roiCorrected).clone();

	cv::Mat cvImageWorking = cvSubImage.clone();

	blurImage(cvSubImage, cvImageWorking, params_.blurImageSize, params_.smoothingAlgorithm);
	if (nodeSelected)
	{
		cv::imwrite("image_real.png", cvSubImage);
		cv::imwrite("image_blurred.png", cvImageWorking);
	}

	thresholdImage(cvImageWorking, cvImageWorking);
	if (nodeSelected)
	{
		cv::imwrite("image_threshold.png", cvImageWorking);
	}

	//find line thickness for sloped horizontal line:
	double angle = nodesSet_.getAngle(row_local, col_local, -1);
	int thicknessLineHorizontal = -1;
	calculateLineThickness(cvImageWorking, SUM_HORIZONTALLY, true, angle, thicknessLineHorizontal);

	//find line thickness for vertical line:
	int thicknessLineVertical = -1;
	calculateLineThickness(cvImageWorking, SUM_VERTICALLY, false, angle, thicknessLineVertical);

	//create mask with angle:
	NodeType nodeType = nodesSet_.getNodeType(row_local, col_local);
	cv::Mat mask;
	cv::Size sizeSubImageCorrected = cvSubImage.size();
	AlgorithmsImages::generateMaskWithAngle(sizeSubImageCorrected / 3, cv::Size(thicknessLineVertical, thicknessLineHorizontal), angle, nodeType, mask);

	blurImage(mask, mask, params_.blurMaskSize, SMOOTHING_GAUSS);

	if (nodeSelected)
	{
		cv::imwrite("image_mask.png", mask);
	}
	
	//calculate subimage extents:
	int leftExtent, rightExtent, topExtent, bottomExtent;
	AlgorithmsImages::getImageExtents(cvImage_, sizeSubImage, node, leftExtent, rightExtent, bottomExtent, topExtent);
	cv::copyMakeBorder(cvSubImage, cvSubImage, topExtent, bottomExtent, leftExtent, rightExtent, cv::BORDER_CONSTANT);

	//calculate cross-correlation matrix and find maximum:
	cv::Mat corrMatrix;
	cv::matchTemplate(cvSubImage, mask, corrMatrix, CV_TM_CCOEFF_NORMED);
	
	//save cross-correlation matrix to file (grayscale and color):
	if (nodeSelected)
	{
		cv::Mat corrMatrix1;
		cv::normalize(corrMatrix, corrMatrix1, 0, 1, cv::NORM_MINMAX);
		corrMatrix1.convertTo(corrMatrix1, CV_8UC1, 255.0);
		cv::imwrite("image_corr.png", corrMatrix1);
		cv::applyColorMap(corrMatrix1, corrMatrix1, cv::COLORMAP_JET);
		cv::imwrite("image_corr_color.png", corrMatrix1);
	}

	//localize maximum of cross-correlation function:
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
	cv::minMaxLoc(corrMatrix, &minVal, &maxVal, &minLoc, &maxLoc);

	//new position of node:
	cv::Point nodeNew = roiCorrected.tl() + cv::Point(-leftExtent, -topExtent) + maxLoc + cv::Point(mask.cols / 2, mask.rows / 2);

	//remember problematic node location:
	cv::Point delta = nodesSetBase_.at(row_local, col_local) - nodeNew;
	double distance = sqrt(delta.x * delta.x + delta.y * delta.y);
	int cellSide = std::min(sizeCell.width, sizeCell.height);

	int maxPosErrorPercents = params_.maxPosError;

	if (distance > (cellSide * (1.0 * maxPosErrorPercents / 100)))
	{
		problemRects_.push_back(cv::Rect(nodeNew - cv::Point(sizeCell / 2), nodeNew + cv::Point(sizeCell / 2)));
	}

	node = nodeNew;

	if (nodeSelected)
	{
		int indexOfRect(-1);
		if (nodeInsideOneOfRects(nodeNew, indexOfRect))
		{
			problemRects_.erase(problemRects_.begin() + indexOfRect);
		}

		emit updateVisualizationS();
	}
	
}

bool Model::nodeInsideOneOfRects(const cv::Point& node, int& indexOfRect)
{
	for (int i = 0; i < problemRects_.size(); i++)
	{
		if (problemRects_[i].contains(node))
		{
			indexOfRect = i;
			return true;
		}
	}
	indexOfRect = -1;
	return false;
}

void Model::averageRect(const QRect& rect)
{
	cv::Rect cRect(rect.x(), rect.y(), rect.width(), rect.height());
	cv::Rect rectCorrected = cRect & cv::Rect(0,0,cvImage_.cols, cvImage_.rows);
	cv::Mat imageFragment = cv::Mat(cvImage_, rectCorrected);
	imageFragment = cv::mean(imageFragment);
	emit updateImageS();
}

void Model::setBackgroundTemplate(const QRect& rect)
{
	cv::Rect roi = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height());
	cv::Mat m = cv::Mat(cvImage_, roi).clone();
	cvBackgroundTemplate_ = m;
}


void Model::pasteBackgroundTemplate(const QRect& rect)
{
	cv::Rect roi = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height());
	roi = roi & cv::Rect(0, 0, cvImage_.cols, cvImage_.rows);
	AlgorithmsImages::fillImageFragmentByTemplate(cvImage_, roi, cvBackgroundTemplate_);
	emit updateImageS();	
}

void Model::calculateCorrectionTable(int iteration)
{
	assert(valid());
	if (!valid())
	{
		return;
	}
	// calculate origin of nodes set:
	const int c_cols = nodesSet_.cols();
	const int c_rows = nodesSet_.rows();
	cv::Point nodeTop = nodesSet_.at(c_rows - 2, c_cols / 2);
	cv::Point nodeBottom = nodesSet_.at(1, c_cols / 2);
	cv::Point nodeLeft = nodesSet_.at(c_rows / 2, 1);
	cv::Point nodeRight = nodesSet_.at(c_rows / 2, c_cols - 2);
	cv::Point originOfNodesSet = nodesSet_.center(); // cv::Point((nodeLeft.x + nodeRight.x) / 2, (nodeBottom.y + nodeTop.y) / 2);

	//calculate direction of main axis of grid:
	std::vector<cv::Point> valuesX = nodesSet_.row(c_rows / 2);
	valuesX = std::vector<cv::Point>(valuesX.begin() + 1, valuesX.end() - 1);
	cv::Vec4d lineOx;
	cv::fitLine(valuesX, lineOx, CV_DIST_L2, 0, 0.01, 0.01);
	cv::Vec2d e1 = cv::Vec2d(lineOx[0], lineOx[1]); // unit ort of "x-axis" of nodes-set
	if (e1[0] < 0)
	{
		e1 = -e1;
	}

	std::vector<cv::Point> valuesY = nodesSet_.col(c_cols / 2);
	valuesY = std::vector<cv::Point>(valuesY.begin() + 1, valuesY.end() - 1);
	cv::Vec4d lineOy;
	cv::fitLine(valuesY, lineOy, CV_DIST_L2, 0, 0.01, 0.01);
	cv::Vec2d e2 = cv::Vec2d(e1[1],-e1[0]); // e2 is orth. to e1
	//cv::Vec2d e2 = cv::Vec2d(lineOy[0], lineOy[1]); // unit ort of "y-axis" of nodes-set
	if (e2[1] > 0)
	{
		e2 = -e2;
	}

	//calculate average size of grid cell:
	const int c_cell_count_x = c_cols - 3;
	const int c_cell_count_y = c_rows - 3;
	const double distHorX =  (nodeRight - nodeLeft).x;
	const double distHorY = (nodeRight - nodeLeft).y;
	const double distVertX = (nodeTop - nodeBottom).x;
	const double distVertY = (nodeTop - nodeBottom).y;

	cv::Size2d cellSize;
	cellSize.width = sqrt(distHorX * distHorX + distHorY * distHorY) / c_cell_count_x;
	cellSize.height = sqrt(distVertX * distVertX + distVertY * distVertY) / c_cell_count_y;

	cv::Matx<double, 3, 3> mx = getScansTrans(nodesSet_, originOfNodesSet); // here we already invert this matrix
	cv::invert(mx, mx); // invert twice -> get initial matrix (?)

	// save data to file:
	//16-bit:
	int indexOriginX = c_cols / 2;
	int indexOriginY = c_rows / 2;
	std::ofstream outStream16("correct_16.dat");
	outStream16 << 2 << std::endl << std::endl;

	//32-bit:
	std::ofstream outStream32("correct_32.dat");
	outStream32 << 3 << std::endl << std::endl;
	for (int row = 0; row < c_rows; row++)
	{
		for (int col = 0; col < c_cols; col++)
		{
			cv::Point shiftFromOriginInt = nodesSet_.at(row, col) - originOfNodesSet;
			cv::Vec2d shiftFromOrigin = cv::Vec2d(shiftFromOriginInt.x, shiftFromOriginInt.y);
			cv::Vec2d shift2D = cv::Vec2d((col - indexOriginX) * ( cellSize.width * e1) + (row - indexOriginY) * (cellSize.height * e2)) - shiftFromOrigin;
			cv::Vec3d shift = cv::Vec3d(shift2D[0], shift2D[1], 0);
			shift = mx * shift;
			if (iteration != 0)
			{
				if (row == 0 || row == c_rows - 1  || col == 0 || col == c_cols - 1)
				{
					shift = 0;
				}
			}
			outStream32 << lround(shift[0]) << ' ' << lround(shift[1]) << std::endl;
			outStream16 << lround(shift[0] / (1 << 16)) << ' ' << lround(shift[1] / (1 << 16)) << std::endl;
		}
		outStream16 << std::endl;
		outStream32 << std::endl;
	}
	outStream16 << std::endl;
	outStream16 << "Origin (pixels): " << '(' << originOfNodesSet.x << ',' << originOfNodesSet.y << ')' << std::endl;
	outStream16 << "Angles (degree): " << std::endl;
	double angleOxAndE1Degrees = Algorithms::angleRadiansToDegrees(std::atan2(e1[1], e1[0]));
	outStream16 << "     " << "Ox and e1: " << angleOxAndE1Degrees << std::endl;
	double angleOxAndE2Degrees = Algorithms::angleRadiansToDegrees(std::atan2(e2[1], e2[0]));
	outStream16 << "     " << "e1 and e2 (abs. value): " << fabs(angleOxAndE2Degrees - angleOxAndE1Degrees) << std::endl;

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

QVector<QRect> Model::getProblemRects() const
{
	QVector<QRect> problemRects;
	for (int i = 0; i < problemRects_.size(); i++)
	{
		const cv::Rect& rect = problemRects_[i];
		problemRects.push_back(QRect(rect.tl().x, rect.tl().y, rect.width, rect.height));
	}
	return problemRects;
}

void Model::setNodePosition(int index, int posX, int posY)
{
	int row = index / nodesSet_.cols();
	int col = index % nodesSet_.cols();
	cv::Point node = cv::Point(posX, posY);
	nodesSet_.setNode(row, col, node);
	indexCur_ = index;
}

bool Model::valid()
{
	assert(!cvImage_.empty());
	assert(!nodesSet_.empty());
	return (!cvImage_.empty() && !nodesSet_.empty());
}

void Model::doOperation(const Operation& operation)
{
	const Parameters& p = params_;

	switch (operation)
	{

	case OPERATION_FIND_NODES_APPROX:
		findNodesApproximately(p.gridRows, p.gridCols, p.peakNeighGlobal);
		break;

	case OPERATION_FIND_NODES_ACCURATE:

		findNodesAccurately(p.gridRows, p.gridCols, p.cellSizeFactor, p.blurImageSize, p.blurMaskSize, p.peakNeighLocal);
		break;
	case OPERATION_FIND_SINGLE_NODE_ACCURATE:

		findSingleNodeAccurately(-1, -1, p.cellSizeFactor, p.blurImageSize, p.blurMaskSize, p.peakNeighLocal, true);
		break;

	default:
		break;
	}
}

cv::Point Model::nodeCur()
{
	assert(indexCur_ != INT_MIN);
	if (indexCur_ == INT_MIN)
	{
		return cv::Point(INT_MIN, INT_MIN);
	}

	int row(-1);
	int col(-1);
	getRowColByIndex(indexCur_, row, col);

	return nodesSet_.at(row, col);
}

void Model::getRowColByIndex(int index, int& row, int& col)
{
	row = index / nodesSet_.cols();
	col = index % nodesSet_.cols();
}

void Model::setParams(const Parameters& params)
{
	params_ = params;
}

void Model::removeProblemRect(int index)
{
	bool indexCorrect = (0 <= index) && (index < problemRects_.size());
	assert(indexCorrect);
	if (indexCorrect)
	{
		problemRects_.erase(problemRects_.begin() + index);
	}
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

void Model::calculateLineThickness(const cv::Mat& cvImage, int sumDirection, bool slopeNeeded, double angle, int& thickness)
{
	std::vector<double> sums;
	if (sumDirection == SUM_HORIZONTALLY)
	{
		if (slopeNeeded)
		{
			AlgorithmsImages::sumIntensityHorizontallyWithSlope(cvImage, angle, sums);
		}
		else
		{
			AlgorithmsImages::sumIntensityHorizontally(cvImage, sums);
		}
	}
	else if (sumDirection == SUM_VERTICALLY)
	{
		if (slopeNeeded) // not done yet! now it is the same logic as without slope
		{
			AlgorithmsImages::sumIntensityVertically(cvImage, sums);
		}
		else
		{
			AlgorithmsImages::sumIntensityVertically(cvImage, sums);
		}
	}
	else
	{
		assert(false);
		return;
	}
	
	AlgorithmsImages::getLineThickness(sums, getPeakNeighborhoodLocal(), getBlurImageSize(), thickness);
}

void Model::thresholdImage(const cv::Mat& src, cv::Mat& dst)
{
	if (params_.thresholdEnabled)
	{
		if (params_.autoThresholdEnabled)
		{
			cv::threshold(src, dst, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		}
		else
		{
			cv::threshold(src, dst, params_.thresholdValue, 255, CV_THRESH_BINARY);
		}
	}
}

void Model::blurImage(const cv::Mat& src, cv::Mat& dst, int blurSize, int smoothingAlgorithm)
{
	if (blurSize > 0) // if blur operator size equals 0 then no blur
	{
		// check for blur operator size. it must be odd!
		if (blurSize % 2 == 0)
		{
			blurSize -= 1;
		}

		if (smoothingAlgorithm == SMOOTHING_GAUSS)
		{
			cv::GaussianBlur(src, dst, cv::Size(blurSize, blurSize), 0, 0);
		}
		else if (smoothingAlgorithm == SMOOTHING_MEDIAN)
		{
			cv::medianBlur(src, dst, blurSize);
		}
	}
}

int Model::getBlurImageSize()
{
	return params_.blurImageSize;
}

int Model::getPeakNeighborhoodLocal()
{
	return params_.peakNeighLocal;
}



void Model::test()
{
	//cv::Mat mask2;
	//AlgorithmsImages::generateMaskWithAngle(sizeSubImageCorrected / 3, cv::Size(1, 1), angle, nodeType, mask2);
	//cv::Mat corrMatrix2;
	//cv::matchTemplate(cvSubImage, mask2, corrMatrix2, CV_TM_CCOEFF_NORMED);
	//cv::normalize(corrMatrix2, corrMatrix2, 0, 1, cv::NORM_MINMAX);
	//corrMatrix2.convertTo(corrMatrix2, CV_8UC1, 255.0);
	//cv::applyColorMap(corrMatrix2, corrMatrix2, cv::COLORMAP_JET);
	//cv::imwrite("image_corr_no_thickness.png", corrMatrix2);

	//cv::Mat mask3;
	//AlgorithmsImages::generateMaskWithAngle(sizeSubImageCorrected / 3, cv::Size(thicknessLineVertical, thicknessLineHorizontal), 0, nodeType, mask3);
	//cv::Mat corrMatrix3;
	//cv::matchTemplate(cvSubImage, mask3, corrMatrix3, CV_TM_CCOEFF_NORMED);
	//cv::normalize(corrMatrix3, corrMatrix3, 0, 1, cv::NORM_MINMAX);
	//corrMatrix3.convertTo(corrMatrix3, CV_8UC1, 255.0);
	//cv::imwrite("image_corr_no_angle.png", corrMatrix3);
	//cv::applyColorMap(corrMatrix3, corrMatrix3, cv::COLORMAP_JET);
	//cv::imwrite("image_corr_no_angle_color.png", corrMatrix3);

	//cv::Mat mask4;
	//AlgorithmsImages::generateMaskWithAngle(sizeSubImageCorrected / 3, cv::Size(1, 1), 0, nodeType, mask4);
	//cv::Mat corrMatrix4;
	//cv::matchTemplate(cvSubImage, mask4, corrMatrix4, CV_TM_CCOEFF_NORMED);
	//cv::normalize(corrMatrix4, corrMatrix4, 0, 1, cv::NORM_MINMAX);
	//corrMatrix4.convertTo(corrMatrix4, CV_8UC1, 255.0);
	//cv::applyColorMap(corrMatrix4, corrMatrix4, cv::COLORMAP_JET);
	//cv::imwrite("image_corr_no_angle_no_thickness.png", corrMatrix4);
}