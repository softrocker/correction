#include "GraphicsScene.h"
#include "Controller.h"
#include "DataTransfer.h"
#include "Model.h"
#include "ImageDisplay.h"
#include "AlgorithmsImages.h"

#include <opencv2/highgui/highgui.hpp>
//#include <QPointF>


Controller::Controller(Model* model, GraphicsScene* scene)
{
	model_ = model;
	scene_ = scene;
	connect(scene, &GraphicsScene::nodePosChangedS, model, &Model::setNodePosition);
	connect(scene, &GraphicsScene::mousePosChangedS, this, &Controller::mousePosChangedS);
	connect(model, &Model::sendProgressS, this, &Controller::sendProgressS);
	connect(model, &Model::operationfinishedS, this, [=] {sendProgressS(0); unblockButtonsS(); });
	connect(model, &Model::updateVisualizationS, this, [=] {scene->addNodesItems(model_->getNodesVisual()); });

	qRegisterMetaType<Operation>("Operation");
	connect(this, &Controller::doOperationS, model, &Model::doOperation); // operations in model work in separate thread
	connect(scene, &GraphicsScene::nodeSelectedS, 
		[=] (int indexNode)
	{ 
		int row(-1);
		int col(-1);
		model->getRowColByIndex(indexNode, row, col);
		emit nodeSelectedS(row, col);
	});
}

Controller::~Controller()
{

}

void Controller::loadImage()
{
	cv::Mat cvImage;
	DataTransfer::loadCvImage(NULL, cvImage);
	if (cvImage.empty())
		return;
	scene_->deleteImageBlocks();
	model_->setImage(cvImage);
	ImageDisplay imageDisplay;
	createVisualImageBlocks(cvImage, imageDisplay);
	scene_->addImageBlocks(imageDisplay);
}

void Controller::createVisualImageBlocks(const cv::Mat& cvImage, ImageDisplay& imageDisplay)
{
	Q_ASSERT(cvImage.size > 0);
	const int c_block_width = 2000;
	const int c_block_height = 2000;
	AlgorithmsImages::createVisualImageBlocks(cvImage, c_block_width, c_block_height, imageDisplay);
}