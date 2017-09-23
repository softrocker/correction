#pragma once

#include <opencv2/core/core.hpp>
#include <QObject>
#include <QVector>
#include <QImage>

#include "NodesSet.h"

struct ImageDisplay;
class ImageController : public QObject
{
	Q_OBJECT
public:
	ImageController(QObject* parent);
	~ImageController();
	void readImage(const std::string& filename);
	void createVisualImageBlocks(ImageDisplay& imageDisplay);
	void findNodesApproximately();

	void findNodesAccurately();
	QVector<QPoint> getNodesVisual() const;
	void test();
	//std::vector<cv::Point> getNodes() const;
private:
	cv::Mat cvImage_;
	NodesSet nodesSet_;
	//std::vector<cv::Point> nodes_;
};
