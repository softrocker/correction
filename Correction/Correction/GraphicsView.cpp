#include "GraphicsView.h"
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QWidget>

GraphicsView::GraphicsView(QWidget* parent) :
    QGraphicsView(parent)
{

}

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent) :
	QGraphicsView(scene, parent)
{
	setMouseTracking(true);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    const double scaleFactor = 1.2;
	double scaleFactorNew = 0;
	if (event->delta() > 0)
	{
		scaleFactorNew = scaleFactor;
	}
	else 
	{
		scaleFactorNew = 1.0 / scaleFactor;
	}
    scale(scaleFactorNew, scaleFactorNew);

	scaleS(scaleFactorNew);
}