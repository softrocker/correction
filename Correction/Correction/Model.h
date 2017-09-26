#pragma once

#include <opencv2/core/core.hpp>
#include <QObject>
#include <QVector>
#include <QImage>

#include "NodesSet.h"

struct ImageDisplay;
class Model : public QObject
{
	Q_OBJECT
public:
	Model(QObject* parent);
	~Model();
	void setImage(const cv::Mat& cvImage); //void readImage(const std::string& filename);
	void findNodesApproximately(int rows, int cols);

	void findNodesAccurately(int rows, int cols);
	QVector<QPoint> getNodesVisual() const;
	void test();

	void setNodePosition(int index, int posX, int posY);

	bool valid();
private:
	void getRowColByIndex(int index, int& row, int& col);
private:
	
	cv::Mat cvImage_;
	NodesSet nodesSet_;
	cv::Point nodeCur_; // special variable for testing
	int indexCur_; // also variable for testing
	//std::vector<cv::Point> nodes_;
};
