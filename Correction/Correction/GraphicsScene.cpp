#include <QGraphicsPixmapItem>
#include "GraphicsScene.h"
#include "ImageDisplay.h"

GraphicsScene::GraphicsScene(QObject* parent) : QGraphicsScene(parent)
{

}

void GraphicsScene::addImageBlocks(const ImageDisplay& imageDisplay)
{
	imageBlockItems_.clear();
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



