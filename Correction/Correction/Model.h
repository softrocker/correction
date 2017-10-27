#pragma once

#include <opencv2/core/core.hpp>
#include <QObject>
#include <QVector>
#include <QImage>

#include "NodesSet.h"
#include "Parameters.h"

struct ImageDisplay;

enum Operation
{
	OPERATION_FIND_NODES_APPROX = 0,
	OPERATION_FIND_NODES_ACCURATE = 1,
	OPERATION_FIND_SINGLE_NODE_ACCURATE = 2,
	OPERATION_WRITE_CORRECTION_TABLE = 3
};


class Model : public QObject
{
	Q_OBJECT
public:
	explicit Model(QObject* parent = 0);
	~Model();
	void setImage(const cv::Mat& cvImage);

	/*
	Algorithm to find approximate positions of nodes.
	rows, cols - number of rows and columns in the grid,
	peakNeighborhoodGlobal - neighborhood of peak to remove
	*/
	void findNodesApproximately(int rows, int cols, int peakNeighborhoodGlobal);

	/*
	Algorithm to find accurate positions of nodes.
	rows, cols - number of rows and columns in the grid,
	cellSizeFactor - parameter which means a part of window, used to construct a subImage by node (if cellSizeFactor == 1.0 then window size is equal to cellsize). 
	blurImage - window size of blur for image
	blurMask - window size of blur for mask
	peakNeighborhoodLocal - neighborhood of peak to remove (used to find line thickness)
	*/
	void findNodesAccurately(int rows, int cols, double cellSizeFactor, int blurImage, int blurMask, int peakNeighborhoodLocal);

	/*
	Algorithm to find accurate position of single node.
	row, col - row and column of current node
	cellSizeFactor, blurImage, blurMask, peakNeighborhoodLocal -- described in commentary of method  "findNodesAccurately()"
	nodeSelected - parameter which means is node selected before by user directly or not
	*/
	void findSingleNodeAccurately(int row, int col, double cellSizeFactor, int blurImage, int blurMask,
		int peakNeighborhoodLocal, bool nodeSelected = false); // if nodeSelected == true => node will be equal to nodeCur_ and row and col will be not used
	
	/* 
	Algorithm to calculate correction table and save it to file/
	iteration - number of iteration (for iteration == 0 all values remains as is, for iteration > 0 boundary nodes coordinantes will be zero)
	*/
	void calculateCorrectionTable(int iteration);

	/*
	Method for testing
	*/
	void test();

	/*
	Method returns Qt representation of nodes
	*/
	QVector<QPoint> getNodesVisual() const; // <------ not needed method in model, TODO replace.

	/*
	Method returns Qt representation of "problem" rects
	*/
	QVector<QRect> getProblemRects() const; // <------ not needed method in model, TODO replace.

	/*
	Method sets position of node with index "index" to (posX, PosY) 
	*/
	void setNodePosition(int index, int posX, int posY);

	/*
	Method executes given opetation. 
	This method is used as a slot for some signal, what allows to run in a separate (not GUI) thread.
	*/
	void doOperation(const Operation& operation);
	
	/*
	Method returns current selected node position
	*/
	cv::Point nodeCur();

	/*
	Method used to calculate row and column of grid node by given index
	*/
	void getRowColByIndex(int index, int& row, int& col);

	/*
	Method sets model parameters for future use in calculation algorithms
	*/
	void setParams(const Parameters& params);
	
	/*
	Method removes problem rect with given index
	*/
	void removeProblemRect(int index);

	bool nodeInsideOneOfRects(const cv::Point& node, int& indexOfRect);

signals:
	void updateVisualizationS();
	void sendProgressS(int progressInPercents);
	void operationfinishedS();

private:
	/*
	First step of calculation of approximate node positions. 
	cvImage - initial big image,
	nodesSet - class contains sought-for nodes positions
	grid_rows, grid_cols - number of rows and columns in grid
	peakNeighborhoodGlobal - neighborhood of peak to remove
	*/
	void clarifyNodes0(const cv::Mat& cvImage, NodesSet& nodesSet, int grid_rows, int grid_cols
		, int peakNeighborhoodGlobal);

	/*
	Second step of calculation of approximate node positions.
	*/
	void clarifyNodes1(const cv::Mat& cvImage, NodesSet& nodesSet, int peakNeighborhoodGlobal);
	/*
	Third step of calculation of approximate node positions.
	*/
	void clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet);
	bool valid();
	
private:
	
	cv::Mat cvImage_; // big image to find nodes 
	NodesSet nodesSet_; // object contains nodes positions
	NodesSet nodesSetBase_; // object contains nodes approximate nodes positions
	std::vector<cv::Rect> problemRects_; // rectangles that shows "problem" nodes positions
	int indexCur_; //index of current selected node
	Parameters params_; // model parameters, used in algorithms
};