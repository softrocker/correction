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
signals:
	void mousePosChangedS(const QPointF& pos);
	void sendProgressS(int progressPercents);
	void nodeSelectedS(int row, int col);
	void unblockButtonsS();
private:
	signals:
	void doOperationS(const Operation& operation, const QVariantList& params);

private:
	GraphicsScene* scene_;
	Model* model_;
};