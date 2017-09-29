#pragma once

#include <QObject>
#include <opencv2/core/core.hpp>

#include "Model.h"

class GraphicsScene;
class ImageDisplay;


class Controller : public QObject
{
	Q_OBJECT
public:
	Controller(Model* model, GraphicsScene* scene);
	//Controller(QWidget* parent, Model* model, GraphicsScene* scene);
	~Controller();
	void loadImage();
	void createVisualImageBlocks(const cv::Mat& cvImage, ImageDisplay& imageDisplay);
	//void findNodesApproximately(int rows_count, int cols_count);
	//void findNodesAccurately(int rows_count, int cols_count);
	//void calculateCorrectionTable();
	void doOperation(Operation operation, const QVariantList& params);
signals:
	void sendProgressS(int progressPercents);
private:
	signals:
	void doOperationS(Operation operation, const QVariantList& params);

private:
	GraphicsScene* scene_;
	Model* model_;
};