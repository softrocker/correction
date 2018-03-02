#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include "GraphicsScene.h"
#include "ImageDisplay.h"
#include "AlgorithmsCalculus.h"

GraphicsScene::GraphicsScene(QObject* parent) : QGraphicsScene(parent) 
{
	nodeSelected = false;
	nodeSelectedIndex = -1;
	scale_ = 1.0;
	instrument_ = InstrumentCursor;
	selectionRectItem_ = new QGraphicsRectItem(QRectF(0,0,0,0));
	QPen pen;
	pen.setWidth(5);
	pen.setColor(QColor(255,255,0));
	selectionRectItem_->setPen(pen);
	selectionRectItem_->setZValue(10);
	addItem(selectionRectItem_);
	selectingMode = false;
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

void GraphicsScene::updateImageBlock(int indexOfBlock, const QImage& imgBlock)
{
	QPointF pos = imageBlockItems_[indexOfBlock]->scenePos();
	removeItem(imageBlockItems_[indexOfBlock]);
	delete imageBlockItems_[indexOfBlock];
	imageBlockItems_[indexOfBlock] = new QGraphicsPixmapItem(QPixmap::fromImage(imgBlock, Qt::AutoColor));
	addItem(imageBlockItems_[indexOfBlock]);
	imageBlockItems_[indexOfBlock]->setPos(pos);
}

void GraphicsScene::updateImageBlocks(const QVector<int>& indexesOfBlocks, const QVector<QImage>& imageBlocks)
{
	for (int i = 0; i < indexesOfBlocks.size(); i++)
	{
		updateImageBlock(indexesOfBlocks[i], imageBlocks[i]);
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
		nodeItem->setTransformOriginPoint(nodeItem->rect().center());
		nodeItem->setZValue(100);
		nodeItem->setScale(scale_);
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

void GraphicsScene::addProblemRectItems(const QVector<QRect>& problemRects)
{
	if (!problemItems_.empty())
	{
		deleteProblemRectItems();
	}
	for (int i = 0; i < problemRects.size(); i++)
	{
		QPen pen(QColor(0, 0, 255));
		pen.setWidth(problemRects[i].width() / 10);
		QGraphicsRectItem* item = addRect(problemRects[i], pen);
		item->setTransformOriginPoint(problemRects[i].center());
		problemItems_.push_back(item);
	}
}

void GraphicsScene::deleteProblemRectItems()
{
	for (int i = 0; i < problemItems_.size(); i++)
	{
		removeItem(problemItems_[i]);
	}
	problemItems_.clear();
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::RightButton)
	{
		return;
	}

	if (instrument_ == InstrumentCursor)
	{
		
		if ((event->modifiers() & Qt::ControlModifier) && (event->button() == Qt::RightButton))
		{
			for (int i = 0; i < problemItems_.size(); i++)
			{
				auto problemItem = problemItems_[i];
				if (problemItem->rect().contains(event->scenePos()))
				{
					emit problemRectDeletedS(i);
					removeItem(problemItem);
					problemItems_.erase(problemItems_.begin() + i);
					return;
				}
			}
		}

		const double c_distance_limit = 10 * scale_;
		for (int i = 0; i < nodesItems_.size(); i++)
		{
			QPointF posNode = nodesItems_[i]->mapToScene(nodesItems_[i]->rect().center());
			nodePosLast = posNode;
			QPointF posEvent = event->scenePos();
			QPointF dPos = posNode - posEvent;
			qreal distance = sqrt(dPos.x() * dPos.x() + dPos.y() * dPos.y());
			if (distance < c_distance_limit)
			{
				nodeSelected = true;
				nodeSelectedIndex = i;
				nodesItems_[i]->setBrush(QColor(0, 255, 0));
				emit nodeSelectedS(nodeSelectedIndex);
				break;
			}
		}
	}
	else if (instrument_ == InstrumentRect)
	{
		selectingMode = true;
		QPointF pos = event->scenePos();
		selectionRectItem_->setRect(QRectF(pos, pos + QPoint(1, 1)));
		selectionRectItem_->update();
		update();
	}
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (instrument_ == InstrumentCursor)
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
	else if (instrument_ == InstrumentRect)
	{
		selectingMode = false;
		
	}
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF mousePos = event->scenePos();
	emit mousePosChangedS(mousePos);
	if (instrument_ == InstrumentCursor)
	{
		if (nodeSelected && (nodeSelectedIndex != -1))
		{
			QPointF dPos = mousePos - nodePosLast;
			nodesItems_[nodeSelectedIndex]->moveBy(dPos.x(), dPos.y());
			nodePosLast = mousePos;
		}
	}
	else if (instrument_ == InstrumentRect)
	{
		if (selectingMode)
		{
			selectionRectItem_->setRect(QRectF(selectionRectItem_->rect().topLeft(), event->scenePos()));
		}
	}
	
}


void GraphicsScene::setScale(double scaleFactor)
{
	Q_ASSERT(!Algorithms::numbersEqual(scaleFactor, 0));
	if (Algorithms::numbersEqual(scaleFactor, 0))
	{
		return;
	}
	scale_ /= scaleFactor; 
	for (int i = 0; i < nodesItems_.size(); i++)
	{
		nodesItems_[i]->setTransformOriginPoint(nodesItems_[i]->rect().center());
		nodesItems_[i]->setScale(scale_);
	}

	QPen pen = selectionRectItem_->pen();
	pen.setWidthF(pen.widthF() / scaleFactor);
	selectionRectItem_->setPen(pen);
}

void GraphicsScene::deleteAllVisualizaton()
{
	deleteImageBlocks();
	deleteNodesItems();
	deleteProblemRectItems();
}

void GraphicsScene::setToolbarInstrument(ToolbarInstrument instrument)
{
	instrument_ = instrument;
}

void GraphicsScene::keyPressEvent(QKeyEvent *keyEvent)
{
	if (instrument_ == InstrumentRect)
	{
		if ((keyEvent->modifiers() & Qt::ControlModifier) && (keyEvent->key() == Qt::Key_A))
		{
			emit averageRectS(selectionRectItem_->rect().toRect().normalized());
		}
		else if ((keyEvent->modifiers() & Qt::ControlModifier) && (keyEvent->key() == Qt::Key_C))
		{
			emit setBackgroundTemplateS(selectionRectItem_->rect().toRect().normalized());
		}
		else if ((keyEvent->modifiers() & Qt::ControlModifier) && (keyEvent->key() == Qt::Key_V))
		{
			emit pasteBackgroundTemplateS(selectionRectItem_->rect().toRect().normalized());
		}
	}
	else if (instrument_ == InstrumentCursor)
	{
		if ((keyEvent->key() == Qt::Key_F) && (keyEvent->modifiers().testFlag(Qt::ControlModifier)))
		{
			emit findSingleNodeS();
		}
	}
}

QVector<int> GraphicsScene::indexesBlocksToUpdate(const QRect& rectUpdate)
{
	QVector<int> indexes;
	for (int i = 0; i < imageBlockItems_.size(); i++)
	{
		int blockWidth = (int)imageBlockItems_[i]->boundingRect().width();
		int blockHeight = (int)imageBlockItems_[i]->boundingRect().height();
		QPoint posRect = imageBlockItems_[i]->scenePos().toPoint();

		QRect boundRect(posRect, (posRect + QPoint(blockWidth, blockHeight) - QPoint(1,1)));
		if (boundRect.intersects(rectUpdate))
		{
			indexes.push_back(i);
		}
	}
	return indexes;
}

QRect GraphicsScene::getSelectionRect()
{
	return selectionRectItem_->rect().toRect().normalized();
}

QRect GraphicsScene::getImageBlockRect(int indexBlock)
{
	int blockWidth = (int)imageBlockItems_[indexBlock]->boundingRect().width();
	int blockHeight = (int)imageBlockItems_[indexBlock]->boundingRect().height();
	QPoint posRect = imageBlockItems_[indexBlock]->scenePos().toPoint();
	QRect boundRect(posRect, (posRect + QPoint(blockWidth, blockHeight) - QPoint(1, 1)));
	return boundRect;
}
