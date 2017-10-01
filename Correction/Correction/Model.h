#pragma once

#include <opencv2/core/core.hpp>
#include <QObject>
#include <QVector>
#include <QImage>

#include "NodesSet.h"

struct ImageDisplay;

enum Operation
{
	OPERATION_FIND_NODES_APPROX = 0,
	OPERATION_FIND_NODES_ACCURATE = 1,
	OPERATION_WRITE_CORRECTION_TABLE = 2
};
Q_DECLARE_METATYPE(Operation)

class Model : public QObject
{
	Q_OBJECT
public:
	explicit Model(QObject* parent = 0);
	~Model();
	void setImage(const cv::Mat& cvImage); //void readImage(const std::string& filename);

	void findNodesApproximately(int rows, int cols);
	void clarifyNodes0(const cv::Mat& cvImage, NodesSet& nodesSet, int grid_rows, int grid_cols);
	void clarifyNodes1(const cv::Mat& cvImage, NodesSet& nodesSet);
	void clarifyNodes2(const cv::Mat& cvImage, NodesSet& nodesSet);

	void findNodesAccurately(int rows, int cols);

	void calculateCorrectionTable(std::vector<double>& correctionTable);

	QVector<QPoint> getNodesVisual() const; // <------ not needed method in model, TODO replace.
	void test();

	void setNodePosition(int index, int posX, int posY);

	bool valid();

	void doOperation(const Operation& operation, const QVariantList& operationParameters);
	void getRowColByIndex(int index, int& row, int& col);

signals:
	void updateVisualizationS();
	void sendProgressS(int progressInPercents);
	void operationfinishedS();

private:
	
private:
	
	cv::Mat cvImage_;
	NodesSet nodesSet_;
	cv::Point nodeCur_; // special variable for testing
	int indexCur_; // also variable for testing
	//std::vector<cv::Point> nodes_;
};

class Worker : public QObject
{

};
