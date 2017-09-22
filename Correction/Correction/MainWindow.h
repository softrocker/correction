#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QGraphicsScene;
class GraphicsView;
class QPushButton;
class GraphicsScene;
class QLabel;

class ImageController;

class MainWindow : public QMainWindow
{
public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();
private:
	void createSettings();
	void createMenu();
	void createActions();
	void createLayouts();
	void createViewsAndScenes();

	void loadImage();
	void saveImage();
	void findNodesApproximately();
	void findNodesAccurately();
	void updateImage();

	void setMousePos(const QPointF& pos);
private:

	ImageController* imageController_;

	GraphicsScene* scene_;
	GraphicsView* view_;
	QImage image_;

	QAction* actionLoadImage_;
	QAction* actionExit_;

	QAction* actionFindNodexApprox_;
	QAction* actionFindNodesAccurately_;

	QLabel* labelX;
	QLabel* labelY;
};

#endif 
