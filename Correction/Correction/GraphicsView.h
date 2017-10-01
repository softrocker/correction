#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class QGraphicsScene;
class QWidget;
class QGraphicsView;

class GraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit GraphicsView (QWidget* parent = 0);
	explicit GraphicsView (QGraphicsScene* scene, QWidget* parent = 0);
signals:
	void scaleS(double scaleFactor);
	void mouseMoveS(const QPointF& point);
	
	
protected:
   virtual void wheelEvent(QWheelEvent* event);
	//void mouseMoveEvent(QMouseEvent *event);
};

#endif