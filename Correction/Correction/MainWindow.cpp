#include <QLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QPushbutton>
#include <QDebug>
#include <QLabel>

#include <iostream>
#include <string>
#include <locale.h>

#include "MainWindow.h"
#include "GraphicsView.h"
#include "GraphicsScene.h"
#include "DataTransfer.h"
#include "ImageController.h"
#include "ImageDisplay.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	createSettings();
	createActions();
	createMenu();
	createViewsAndScenes();
	createLayouts();

	image_ = QImage();
	imageController_ = new ImageController(this);

	connect(view_, &GraphicsView::mouseMoveS, this, &MainWindow::setMousePos);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createSettings()
{
	setlocale(LC_ALL, "russian");
}

void MainWindow::createActions()
{
	//file actions:
	actionLoadImage_ = new QAction(tr("&Load image..."), this);
	actionLoadImage_->setStatusTip(tr("Load image to file"));
	connect(actionLoadImage_, &QAction::triggered, this, &MainWindow::loadImage);

	actionExit_ = new QAction(tr("&Exit"), this);
	actionExit_->setStatusTip(tr("Exit"));
	connect(actionExit_, &QAction::triggered, this, &MainWindow::close);

	//operation actions:
	actionFindNodexApprox_ = new QAction(tr("&Find nodes approximately"), this);
	actionFindNodexApprox_->setStatusTip(tr("Find nodes approximately"));
	connect(actionFindNodexApprox_, &QAction::triggered, this, &MainWindow::findNodesApproximately);

	actionFindNodesAccurately_ = new QAction(tr("&Find nodes accurately"), this);
	actionFindNodesAccurately_->setStatusTip(tr("Find nodes accurately"));
	connect(actionFindNodesAccurately_, &QAction::triggered, this, &MainWindow::findNodesAccurately);

	actionTest_ = new QAction(tr("&Test"), this);
	connect(actionTest_, &QAction::triggered, this, &MainWindow::test);
}


void::MainWindow::createViewsAndScenes()
{
	scene_ = new GraphicsScene(this);
	view_ = new GraphicsView(scene_);
	view_->setDragMode(QGraphicsView::ScrollHandDrag);
}

void MainWindow::createLayouts()
{
	QWidget* mainWidget = new QWidget(this);
	setCentralWidget(mainWidget);

	QLayout* imagesLayout = new QHBoxLayout();
	imagesLayout->addWidget(view_);

	QHBoxLayout* coordinatesLayout = new QHBoxLayout();
	coordinatesLayout->addStretch();

	labelX = new QLabel(this);
	labelX->setFixedWidth(100);
	coordinatesLayout->addWidget(labelX);

	labelY = new QLabel(this);
	labelY->setFixedWidth(100);
	coordinatesLayout->addWidget(labelY);


	QLayout* mainLayout = new QVBoxLayout();
	mainLayout->addItem(imagesLayout);
	mainLayout->addItem(coordinatesLayout);
	mainWidget->setLayout(mainLayout);
}

void MainWindow::createMenu()
{
	QMenu* menuMain = menuBar()->addMenu(tr("File"));
	menuMain->addAction(actionLoadImage_);
	//menuMain->addAction(actionSaveImage_);
	menuMain->addAction(actionExit_);

	QMenu* menuOperations = menuBar()->addMenu(tr("Operations"));
	menuOperations->addAction(actionFindNodexApprox_);
	menuOperations->addAction(actionFindNodesAccurately_);
	menuOperations->addAction(actionTest_);
}

void MainWindow::loadImage()
{
	QString imageName = DataTransfer::imageName(this);
	if (imageName.isEmpty())
		return;
	scene_->clear();
	imageController_->readImage(imageName.toStdString());
	ImageDisplay imageDisplay;
	imageController_->createVisualImageBlocks(imageDisplay);
	scene_->addImageBlocks(imageDisplay);	
}

void MainWindow::saveImage()
{
	DataTransfer::saveImage(this, image_);
}

void MainWindow::findNodesApproximately()
{
	imageController_->findNodesApproximately();
	scene_->addNodesItems(imageController_->getNodesVisual());
}

void MainWindow::findNodesAccurately()
{
	imageController_->findNodesAccurately();
	scene_->deleteNodesItems();
	scene_->addNodesItems(imageController_->getNodesVisual());
}

void MainWindow::updateImage()
{
	scene_->update();
}

void MainWindow::setMousePos(const QPointF& pos)
{
	labelX->setText("x = " + QString().setNum(pos.x(), 'f', 3));
	labelY->setText("y = " + QString().setNum(pos.y(), 'f', 3));
}

void MainWindow::test()
{
	imageController_->test();
}