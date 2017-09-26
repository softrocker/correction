#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include "GraphicsScene.h"
#include "ImageDisplay.h"

GraphicsScene::GraphicsScene(QObject* parent) : QGraphicsScene(parent)
{
	nodeSelected = false;
	nodeSelectedIndex = -1;
}

void GraphicsScene::addImageBlocks(const ImageDisplay& imageDisplay)
{
	if (!imageBlockItems_.empty())
	{
		deleteImageBlocks();
	}
	const int c_blocks_count_x = imageDisplay.blocksCountX;
	const int c_blocks_count_y = imageDisplay.blocksCountY;
	for (int row = 0; row < c_blocks_count_y; row++)
	{
		for (int col = 0; col < c_blocks_count_x; col++)
		{
			const QImage& imgBlock = imageDisplay.imageBlocks[c_blocks_count_x * row + col];
			QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(imgBlock, Qt::AutoColor));
			imageBlockItems_.push_back(pixmapItem);
			addItem(pixmapItem);
			pixmapItem->setPos(col * imageDisplay.blockWidth, row * imageDisplay.blockHeight);
		}
	}
}

void GraphicsScene::deleteImageBlocks()
{
	for (int i = 0; i < imageBlockItems_.size(); i++)
	{
		removeItem(imageBlockItems_[i]); // remove from scene
		delete imageBlockItems_[i]; // clear allocated memory
	}
	imageBlockItems_.clear(); 
}

void GraphicsScene::addNodesItems(const QVector<QPoint>& nodesPositions)
{
	if (!nodesItems_.empty())
	{
		deleteNodesItems();
	}
	for (int i = 0; i < nodesPositions.size(); i++)
	{
		QGraphicsEllipseItem* nodeItem = addEllipse(QRect(nodesPositions[i].x()-5, nodesPositions[i].y()-5, 10, 10), QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)));
		addItem(nodeItem);
		nodesItems_.push_back(nodeItem);
	}
}

void GraphicsScene::deleteNodesItems()
{
	for (int i = 0; i < nodesItems_.size(); i++)
	{
		removeItem(nodesItems_[i]);
	}
	nodesItems_.clear();
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	const int c_distance_limit = 10;
	for (int i = 0; i < nodesItems_.size(); i++)
	{
		QPointF posNode =  nodesItems_[i]->mapToScene(nodesItems_[i]->rect().center());
		nodePosLast = posNode;
		QPointF posEvent = event->scenePos();
		QPointF dPos = posNode - posEvent;
		qreal distance = sqrt(dPos.x() * dPos.x() + dPos.y() * dPos.y());
		if (distance < c_distance_limit)
		{
			nodeSelected = true;
			nodeSelectedIndex = i;
			nodesItems_[i]->setBrush(QColor(0, 255, 0));
			break;
		}
	}
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	nodeSelected = false;
	if (nodeSelectedIndex != -1)
	{
		QGraphicsEllipseItem* nodeItem = nodesItems_[nodeSelectedIndex];
		nodeItem->setBrush(QColor(255, 0, 0));
		QPointF nodePos = nodeItem->mapToScene(nodeItem->rect().center());
		emit nodePosChangedS(nodeSelectedIndex, static_cast<int>(nodePos.x()), static_cast<int>(nodePos.y()));
	}
	nodeSelectedIndex = -1;
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (nodeSelected && (nodeSelectedIndex != -1))
	{
		QPointF dPos = event->scenePos() - nodePosLast;
		nodesItems_[nodeSelectedIndex]->moveBy(dPos.x(), dPos.y());
		nodePosLast = event->scenePos();
	}
}