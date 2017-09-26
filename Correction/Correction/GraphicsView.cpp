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
    if(event->delta() > 0)
        scale(scaleFactor, scaleFactor);
    else
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}


//void GraphicsView::mouseMoveEvent(QMouseEvent *event)
//{
//	emit mouseMoveS(mapToScene(event->pos()));
//}