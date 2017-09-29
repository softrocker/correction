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
	model_ = new Model(this);
	controller_ = new Controller(model_, scene_);
	createSettings();
	createActions();
	createMenu();
	createLayouts();
	loadSettings();

	connect(view_, &GraphicsView::mouseMoveS, this, &MainWindow::setMousePos);
	connect(this, &MainWindow::applySettingsS, parametersWidget_, &ParametersWidget::setParameters);
	connect(parametersWidget_, &ParametersWidget::parametersChangedS, this, &MainWindow::setParameters);
	connect(controller_, &Controller::sendProgressS, this, &MainWindow::setProgress);
	//connect(this, &MainWindow::close, this, [&]() { saveSettings(); return true; });

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
		controller_->doOperation(OPERATION_FIND_NODES_APPROX, operationParams);
	});

	actionFindNodesAccurately_ = new QAction(tr("&Find nodes accurately"), this);
	actionFindNodesAccurately_->setStatusTip(tr("Find nodes accurately"));
	connect(actionFindNodesAccurately_, &QAction::triggered, controller_, 
		[&](bool checked)
	{
		QVariantList operationParams;
		operationParams.push_back(params_.gridRows);
		operationParams.push_back(params_.gridCols);
		controller_->doOperation(OPERATION_FIND_NODES_ACCURATE, operationParams);
	});

	actionWriteTable_ = new QAction(tr("&Write correction table"), this);
	actionWriteTable_->setStatusTip(tr("Write correction table"));
	connect(actionWriteTable_, &QAction::triggered, controller_, [&](bool checked) {controller_->doOperation(OPERATION_WRITE_CORRECTION_TABLE, QVariantList()); });

	actionTest_ = new QAction(tr("&Test"), this);
	connect(actionTest_, &QAction::triggered, this, &MainWindow::test);
}


void::MainWindow::createViewsAndScenes()
{
	scene_ = new GraphicsScene(this);
	view_ = new GraphicsView(scene_);
	//view_->setDragMode(QGraphicsView::ScrollHandDrag);
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

	QMenu* menuOperations = menuBar()->addMenu(tr("Operations"));
	menuOperations->addAction(actionFindNodexApprox_);
	menuOperations->addAction(actionFindNodesAccurately_);
	menuOperations->addAction(actionWriteTable_);
	menuOperations->addAction(actionTest_);
	//actionTest_->setVisible(false);
}

void MainWindow::setMousePos(const QPointF& pos)
{
	labelX_->setText("x = " + QString().setNum(pos.x(), 'f', 3));
	labelY_->setText("y = " + QString().setNum(pos.y(), 'f', 3));
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
}

void MainWindow::saveSettings()
{
	QSettings settings("settings", QSettings::IniFormat);
	settings.setValue("grid_cols", params_.gridCols);
	settings.setValue("grid_rows", params_.gridRows);
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
