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
	//emit model_->updateVisualizationS();
}

void Controller::createVisualImageBlocks(const cv::Mat& cvImage, ImageDisplay& imageDisplay)
{
	Q_ASSERT(cvImage.size > 0);
	const int c_block_width = 2000;
	const int c_block_height = 2000;
	AlgorithmsImages::createVisualImageBlocks(cvImage, c_block_width, c_block_height, imageDisplay);
}

//void Controller::doOperation(const Operation& operation)
//{
//	switch (operation)
//	{
//
//	case OPERATION_FIND_NODES_APPROX:
//		findNodesApproximately(rows_count, cols_count);
//		break;
//
//	case OPERATION_FIND_NODES_ACCURATE:
//		assert(operationParameters.size() == 5);
//		if (operationParameters.size() != 5)
//		{
//			return;
//		}
//		rows_count = operationParameters[0].toInt();
//		cols_count = operationParameters[1].toInt();
//		cell_size_factor = operationParameters[2].toDouble();
//		blurImage = operationParameters[3].toInt();
//		blurMask = operationParameters[4].toInt();
//		findNodesAccurately(rows_count, cols_count, cell_size_factor, blurImage, blurMask);
//		break;
//	case OPERATION_FIND_SINGLE_NODE_ACCURATE:
//		assert(operationParameters.size() == 3);
//		if (operationParameters.size() != 3)
//		{
//			return;
//		}
//		cell_size_factor = operationParameters[0].toDouble();
//		blurImage = operationParameters[1].toInt();
//		blurMask = operationParameters[2].toInt();
//		findSingleNodeAccurately(-1, -1, cell_size_factor, blurImage, blurMask, true);
//		break;
//
//	case OPERATION_WRITE_CORRECTION_TABLE:
//		assert(operationParameters.size() == 1);
//		if (operationParameters.size() != 1)
//		{
//			return;
//		}
//		iteration = operationParameters[0].toInt();
//
//		calculateCorrectionTable(iteration);
//		break;
//
//	default:
//		break;
//	}
//}