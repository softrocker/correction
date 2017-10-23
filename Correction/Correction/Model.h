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
	void setImage(const cv::Mat& cvImage); //void readImage(const std::string& filename);

	void findNodesApproximately(int rows, int cols, int peakNeighborhoodGlobal);
	void findNodesAccurately(int rows, int cols, double cellSizeFactor, int blurImage, int blurMask, int peakNeighborhoodLocal);
	void findSingleNodeAccurately(int row, int col, double cellSizeFactor, int blurImage, int blurMask,
		int peakNeighborhoodLocal, bool nodeSelected = false); // if nodeSelected == true => node will be equal to nodeCur_ and row and col will be not used
	bool nodeInsideOneOfRects(const cv::Point& node, int& indexOfRect);
	void calculateCorrectionTable(int iteration);
	void test();

	QVector<QPoint> getNodesVisual() const; // <------ not needed method in model, TODO replace.
	QVector<QRect> getProblemRects()const;

	void setNodePosition(int index, int posX, int posY);

	//void doOperation(const Operation& operation, const QVariantList& operationParameters);
	void doOperation(const Operation& operation);
	
	cv::Point nodeCur();

	void getRowColByIndex(int index, int& row, int& col);

	void setParams(const Parameters& params);

	void removeProblemRect(int index);

signals:
	void updateVisualizationS();
	void sendProgressS(int progressInPercents);
	void operationfinishedS();

private:
	void clarifyNodes0(const cv::Mat& cvImage, NodesSet& nodesSet, int grid_rows, int grid_cols
		, int peakNeighborhoodGlobal);
	void clarifyNodes1(const cv::Mat& cvImage, NodesSet& nodesSet, int peakNeighborhoodGlobal);
	void clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet);
	bool valid();
	
private:
	
	cv::Mat cvImage_;
	NodesSet nodesSet_;
	NodesSet nodesSetBase_;
	std::vector<cv::Rect>problemRects_;
	int indexCur_; // variable for testing
	Parameters params_;
};