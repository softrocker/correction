#include "GraphicsScene.h"
#include "Controller.h"
#include "DataTransfer.h"
#include "Model.h"
#include "ImageDisplay.h"
#include "AlgorithmsImages.h"

#include <opencv2/highgui/highgui.hpp>

Controller::Controller(Model* model, GraphicsScene* scene)
{
	model_ = model;
	scene_ = scene;
	connect(scene, &GraphicsScene::nodePosChangedS, model, &Model::setNodePosition);
	connect(scene, &GraphicsScene::mousePosChangedS, this, &Controller::mousePosChangedS);
	connect(model, &Model::sendProgressS, this, &Controller::sendProgressS);
	connect(model, &Model::operationfinishedS, this, [=] {sendProgressS(0); unblockButtonsS(); });
	connect(model, &Model::updateVisualizationS, this, 
		[=] 
	{
		scene->addNodesItems(model_->getNodesVisual());
		scene->addProblemRectItems(model->getProblemRects());
	});

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

	connect(scene, &GraphicsScene::problemRectDeletedS, model, &Model::removeProblemRect);
	connect(scene, &GraphicsScene::averageRectS, model, &Model::averageRect);
	connect(model, &Model::updateImageS, this,  &Controller::updateImageBlocks); // &Controller::updateImage);
	connect(scene, &GraphicsScene::findSingleNodeS, this, &Controller::findSingleNodeS);
	connect(scene, &GraphicsScene::setBackgroundTemplateS, model, &Model::setBackgroundTemplate);
	connect(scene, &GraphicsScene::pasteBackgroundTemplateS, model, &Model::pasteBackgroundTemplate);
}

Controller::~Controller()
{

}

void Controller::loadImage()
{
	cv::Mat cvImage;
	QString imgName = DataTransfer::imageName(NULL);
	if (imgName.isEmpty())
	{
		return;
	}
	scene_->deleteAllVisualizaton();
	DataTransfer::loadCvImage(NULL, imgName, cvImage);
	if (cvImage.empty())
	{
		return;
	}
	model_->setImage(cvImage);
	ImageDisplay imageDisplay;
	createVisualImageBlocks(cvImage, imageDisplay);
	scene_->addImageBlocks(imageDisplay);

}

void Controller::updateImage()
{
	scene_->deleteImageBlocks();
	ImageDisplay imageDisplay;
	createVisualImageBlocks(model_->getImage(), imageDisplay);
	scene_->addImageBlocks(imageDisplay);
}

void Controller::updateImageBlocks()
{
	auto imageBlockRects = scene_->indexesBlocksToUpdate(scene_->getSelectionRect());
	QVector<QRect> rectsToUpdate;
	rectsToUpdate.reserve(imageBlockRects.size());
	for (int i = 0; i < imageBlockRects.size(); i++)
	{
		rectsToUpdate.push_back(scene_->getImageBlockRect(imageBlockRects[i]));
	}

	std::vector<cv::Mat> imageBlocks;
	model_->calculateBlocksToUpdate(rectsToUpdate, imageBlocks);

	QVector<QImage> qimagesBlocks;
	qimagesBlocks.reserve(imageBlocks.size());
	for (int i = 0; i < imageBlocks.size(); i++)
	{
		cv::Mat m = imageBlocks[i];
		QImage image = AlgorithmsImages::Mat2QImageGray(imageBlocks[i]);
		qimagesBlocks.push_back(image);
	}

	scene_->updateImageBlocks(imageBlockRects, qimagesBlocks);
}

void Controller::createVisualImageBlocks(const cv::Mat& cvImage, ImageDisplay& imageDisplay)
{
	Q_ASSERT(cvImage.size > 0);
	const int c_block_width = 2000;
	const int c_block_height = 2000;
	AlgorithmsImages::createVisualImageBlocks(cvImage, c_block_width, c_block_height, imageDisplay);
}
