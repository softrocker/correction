#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>



class QGraphicsScene;
class GraphicsView;
class QPushButton;
class GraphicsScene;
class QLabel;
class QProgressBar;

class Model;
class Controller;
class ParametersWidget;

#include "Parameters.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();

signals:
	void applySettingsS(const Parameters& params);
private:
	void createSettings();
	void createMenu();
	void createActions();
	void createLayouts();
	void createViewsAndScenes();
	void setMousePos(const QPointF& pos);
	void nodeSelected(int row, int col);
	void loadSettings();
	void saveSettings();
	void applyParameters();
	void test();
	void closeEvent(QCloseEvent *event);
	void setParameters(const Parameters& params);
	void setProgress(int progressPercents);
	void blockButtons(bool block);
	virtual	void keyPressEvent(QKeyEvent *event);
private:
	Controller* controller_;

	Model* model_;

	GraphicsScene* scene_;
	GraphicsView* view_;

	QAction* actionLoadImage_;
	QAction* actionExit_;

	QAction* actionFindNodexApprox_;
	QAction* actionFindNodesAccurately_;
	QAction* actionFindSingleNodeAccurately_;
	QMenu* menuWriteTable_;
	QAction* actionIter0_;
	QAction* actionIter1_;
	QAction* actionTest_;

	QLabel* labelX_;
	QLabel* labelY_;
	QLabel* labelNodeRow_;
	QLabel* labelNodeCol_;

	QProgressBar* progressBar_;

	ParametersWidget* parametersWidget_;

	Parameters params_;

	QThread* threadWork_;
};

#endif 
