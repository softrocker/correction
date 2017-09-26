#pragma once

#include <QObject>
#include <opencv2/core/core.hpp>

class GraphicsScene;
class Model;
class ImageDisplay;


class Controller : public QObject
{
	Q_OBJECT
public:
	Controller(Model* model, GraphicsScene* scene);
	~Controller();
	void loadImage();
	void createVisualImageBlocks(const cv::Mat& cvImage, ImageDisplay& imageDisplay);
	void findNodesApproximately(int rows_count, int cols_count);
	void findNodesAccurately(int rows_count, int cols_count);

private:
	GraphicsScene* scene_;
	Model* model_;
};