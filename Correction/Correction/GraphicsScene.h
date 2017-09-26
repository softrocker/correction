#pragma once

#include <QGraphicsScene>

class QGraphicsPixmapItem;
struct ImageDisplay;

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	GraphicsScene(QObject* parent = 0);
	void addImageBlocks(const ImageDisplay& imageDisplay);
	void deleteImageBlocks();
	void addNodesItems(const QVector<QPoint>& nodesPositions);
	void deleteNodesItems();
signals:
	void nodePosChangedS(int index, int posX, int posY);
protected:
	virtual	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
	QVector<QGraphicsPixmapItem*> imageBlockItems_;
	QVector<QGraphicsEllipseItem*> nodesItems_;
	bool nodeSelected;
	int nodeSelectedIndex;
	QPointF nodePosLast;
};