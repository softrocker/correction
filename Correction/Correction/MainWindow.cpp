#include <QLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QPushbutton>
#include <QDebug>
#include <QLabel>
#include <QSettings>
#include <QProgressBar>
#include <QThread>

#include <iostream>
#include <string>
#include <locale.h>

#include "MainWindow.h"
#include "GraphicsView.h"
#include "GraphicsScene.h"
#include "DataTransfer.h"
#include "Model.h"
#include "ImageDisplay.h"
#include "Controller.h"
#include "ParametersWidget.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	
	createViewsAndScenes();
	model_ = new Model();
	threadWork_ = new QThread(this);
	connect(this, &MainWindow::destroyed, threadWork_, &QThread::quit);
	model_->moveToThread(threadWork_);
	threadWork_->start();
	controller_ = new Controller(model_, scene_);
	createSettings();
	createActions();
	createMenu();
	createLayouts();
	loadSettings();

	connect(controller_, &Controller::mousePosChangedS, this, &MainWindow::setMousePos);
	connect(controller_, &Controller::nodeSelectedS, this, &MainWindow::nodeSelected);
	connect(this, &MainWindow::applySettingsS, parametersWidget_, &ParametersWidget::setParameters);
	connect(parametersWidget_, &ParametersWidget::parametersChangedS, this, &MainWindow::setParameters);
	connect(controller_, &Controller::sendProgressS, this, &MainWindow::setProgress);
	connect(controller_, &Controller::unblockButtonsS, this, [=] {blockButtons(false);});

	applyParameters();

	
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
	connect(actionLoadImage_, &QAction::triggered, controller_, &Controller::loadImage);

	actionExit_ = new QAction(tr("&Exit"), this);
	actionExit_->setStatusTip(tr("Exit"));
	connect(actionExit_, &QAction::triggered, this, &MainWindow::close);

	//operation actions:
	actionFindNodexApprox_ = new QAction(tr("&Find nodes approximately"), this);
	actionFindNodexApprox_->setStatusTip(tr("Find nodes approximately"));
	connect(actionFindNodexApprox_, &QAction::triggered, controller_, [&](bool checked)
	{
		QVariantList operationParams;
		operationParams.push_back(params_.gridRows);
		operationParams.push_back(params_.gridCols);
		controller_->doOperationS(OPERATION_FIND_NODES_APPROX, operationParams);
		blockButtons(true);
	});

	actionFindNodesAccurately_ = new QAction(tr("&Find nodes accurately"), this);
	actionFindNodesAccurately_->setStatusTip(tr("Find nodes accurately"));
	connect(actionFindNodesAccurately_, &QAction::triggered, controller_, 
		[&](bool checked)
	{
		QVariantList operationParams;
		operationParams.push_back(params_.gridRows);
		operationParams.push_back(params_.gridCols);
		operationParams.push_back(params_.cellSizeFactor);
		operationParams.push_back(params_.blurImage);
		operationParams.push_back(params_.blurMask);
		controller_->doOperationS(OPERATION_FIND_NODES_ACCURATE, operationParams);
		blockButtons(true);
	});

	actionFindSingleNodeAccurately_ = new QAction(tr("Find single node accurately"), this);
	actionFindSingleNodeAccurately_->setStatusTip(tr("Find single node accurately"));
	connect(actionFindSingleNodeAccurately_, &QAction::triggered, controller_,
		[&](bool checked)
	{
		QVariantList operationParams; // empty parameters list
		operationParams.push_back(params_.cellSizeFactor);
		operationParams.push_back(params_.blurImage);
		operationParams.push_back(params_.blurMask);
		controller_->doOperationS(OPERATION_FIND_SINGLE_NODE_ACCURATE, operationParams);
	});



	actionIter0_ = new QAction(tr("Itetation 0"), this);
	actionIter1_ = new QAction(tr("Itetation 1"), this);

	connect(actionIter0_, &QAction::triggered, controller_,
		[&](bool checked)
	{
		QVariantList operationParams;
		operationParams.push_back(0);
		controller_->doOperationS(OPERATION_WRITE_CORRECTION_TABLE, operationParams);
	});

	connect(actionIter1_, &QAction::triggered, controller_,
		[&](bool checked)
	{
		QVariantList operationParams;
		operationParams.push_back(1);
		controller_->doOperationS(OPERATION_WRITE_CORRECTION_TABLE, operationParams);
	});

	actionTest_ = new QAction(tr("&Test"), this);
	connect(actionTest_, &QAction::triggered, this, &MainWindow::test);
}


void::MainWindow::createViewsAndScenes()
{
	scene_ = new GraphicsScene(this);
	view_ = new GraphicsView(scene_);
	view_->setDragMode(QGraphicsView::ScrollHandDrag);
	connect(view_, &GraphicsView::scaleS, scene_, &GraphicsScene::setScale);
}

void MainWindow::createLayouts()
{
	QWidget* mainWidget = new QWidget(this);
	setCentralWidget(mainWidget);

	QLayout* imagesLayout = new QHBoxLayout();
	imagesLayout->addWidget(view_);
	parametersWidget_ = new ParametersWidget(this);
	imagesLayout->addWidget(parametersWidget_);

	QHBoxLayout* coordinatesLayout = new QHBoxLayout();
	coordinatesLayout->addStretch();

	labelX_ = new QLabel(this);
	labelX_->setFixedWidth(100);
	coordinatesLayout->addWidget(labelX_);

	labelY_ = new QLabel(this);
	labelY_->setFixedWidth(100);
	coordinatesLayout->addWidget(labelY_);

	labelNodeCol_ = new QLabel(this);
	labelNodeCol_->setFixedWidth(70);
	coordinatesLayout->addWidget(labelNodeCol_);

	labelNodeRow_ = new QLabel(this);
	labelNodeRow_->setFixedWidth(70);
	coordinatesLayout->addWidget(labelNodeRow_);


	QLabel* labelOperationProgress = new QLabel(tr("Operation progress: "), this);
	coordinatesLayout->addWidget(labelOperationProgress);

	progressBar_ = new QProgressBar(this);
	progressBar_->setFixedWidth(200);
	progressBar_->setRange(0, 100);

	coordinatesLayout->addWidget(progressBar_);
	coordinatesLayout->addStretch();

	QLayout* mainLayout = new QVBoxLayout();
	mainLayout->addItem(imagesLayout);
	mainLayout->addItem(coordinatesLayout);
	mainWidget->setLayout(mainLayout);
}

void MainWindow::createMenu()
{
	QMenu* menuMain = menuBar()->addMenu(tr("File"));
	menuMain->addAction(actionLoadImage_);
	menuMain->addAction(actionExit_);

	menuWriteTable_ = new QMenu(tr("&Write correction table"), this);
	menuWriteTable_->setStatusTip(tr("Write correction table"));
	menuWriteTable_->addAction(actionIter0_);
	menuWriteTable_->addAction(actionIter1_);

	QMenu* menuOperations = menuBar()->addMenu(tr("Operations"));
	menuOperations->addAction(actionFindNodexApprox_);
	menuOperations->addAction(actionFindNodesAccurately_);
	menuOperations->addAction(actionFindSingleNodeAccurately_);
	menuOperations->addMenu(menuWriteTable_);
	menuOperations->addAction(actionTest_);
	
	//actionTest_->setVisible(false);
}

void MainWindow::setMousePos(const QPointF& pos)
{
	labelX_->setText("x = " + QString().setNum(pos.x(), 'f', 3));
	labelY_->setText("y = " + QString().setNum(pos.y(), 'f', 3));
}

void MainWindow::nodeSelected(int row, int col)
{
	labelNodeRow_->setText("Row = " + QString().setNum(row));
	labelNodeCol_->setText("Col = " + QString().setNum(col));
}

void MainWindow::loadSettings()
{
	QSettings settings("settings", QSettings::IniFormat);
	if (settings.contains("grid_cols"))
	{
		params_.gridCols = settings.value("grid_cols").toInt();
	}
	if (settings.contains("grid_rows"))
	{
		params_.gridRows = settings.value("grid_rows").toInt();
	}
	if (settings.contains("cell_size_factor"))
	{
		params_.cellSizeFactor = settings.value("cell_size_factor").toDouble();
	}
	if (settings.contains("blur_image"))
	{
		params_.blurImage = settings.value("blur_image").toInt();
	}
	if (settings.contains("blur_mask"))
	{
		params_.blurMask = settings.value("blur_mask").toInt();
	}
}

void MainWindow::saveSettings()
{
	QSettings settings("settings", QSettings::IniFormat);
	settings.setValue("grid_cols", params_.gridCols);
	settings.setValue("grid_rows", params_.gridRows);
	settings.setValue("cell_size_factor", params_.cellSizeFactor);
	settings.setValue("blur_image", params_.blurImage);
	settings.setValue("blur_mask", params_.blurMask);
}

void MainWindow::applyParameters()
{
	if (params_.valid())
	{
		emit applySettingsS(params_);
	}
}

void MainWindow::test()
{
	model_->test();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	saveSettings();
}

void MainWindow::setParameters(const Parameters& params)
{
	params_ = params;
}

void MainWindow::setProgress(int progressPercents)
{
	progressBar_->setValue(progressPercents);
}

void MainWindow::blockButtons(bool block)
{
	actionLoadImage_->setDisabled(block);
	actionFindNodexApprox_->setDisabled(block);
	actionFindNodesAccurately_->setDisabled(block);
	actionFindSingleNodeAccurately_->setDisabled(block);
	actionTest_->setDisabled(block);
	actionIter0_->setDisabled(block);
	actionIter1_->setDisabled(block);
}
