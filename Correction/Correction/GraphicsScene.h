#pragma once

#include <QGraphicsScene>

class QGraphicsPixmapItem;
struct ImageDisplay;
class QPointF;

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	GraphicsScene(QObject* parent = 0);
	void addImageBlocks(const ImageDisplay& imageDisplay);
	void deleteImageBlocks();
	void addNodesItems(const QVector<QPoint>& nodesPositions);
	void deleteNodesItems();
	void addProblemRectItems(const QVector<QRect>& problemRects);
	void deleteProblemRectItems();
	void setScale(double scale);
	void deleteAllVisualizaton();
signals:
	
	void problemRectDeletedS(int indexRect);
	void nodeSelectedS(int indexNode);
	void nodePosChangedS(int index, int posX, int posY);
	void mousePosChangedS(const QPointF& pos);

protected:
	virtual	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
	QVector<QGraphicsPixmapItem*> imageBlockItems_;
	QVector<QGraphicsEllipseItem*> nodesItems_;
	QVector<QGraphicsRectItem*> problemItems_;
	bool nodeSelected;
	int nodeSelectedIndex;
	double scale_;
	QPointF nodePosLast;
};