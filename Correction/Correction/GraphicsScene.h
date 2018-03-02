#pragma once

#include <QGraphicsScene>

class QGraphicsPixmapItem;
struct ImageDisplay;
class QPointF;

enum ToolbarInstrument
{
	InstrumentCursor = 0,
	InstrumentRect = 1
};

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	GraphicsScene(QObject* parent = 0);
	void addImageBlocks(const ImageDisplay& imageDisplay);
	void deleteImageBlocks();
	void updateImageBlock(int indexOfBlock, const QImage& imgBlock);
	void updateImageBlocks(const QVector<int>& indexesOfBlocks, const QVector<QImage>& imageBlocks);
	void addNodesItems(const QVector<QPoint>& nodesPositions);
	void deleteNodesItems();
	void addProblemRectItems(const QVector<QRect>& problemRects);
	void deleteProblemRectItems();
	void setScale(double scale);
	void deleteAllVisualizaton();
	void setToolbarInstrument(ToolbarInstrument instrument);
	void keyPressEvent(QKeyEvent *keyEvent);
	QVector<int> indexesBlocksToUpdate(const QRect& rectUpdate);
	QRect getSelectionRect();
	QRect getImageBlockRect(int indexBlock);
signals:
	
	void problemRectDeletedS(int indexRect);
	void nodeSelectedS(int indexNode);
	void nodePosChangedS(int index, int posX, int posY);
	void mousePosChangedS(const QPointF& pos);
	void averageRectS(const QRect& rect);
	void findSingleNodeS();
	void setBackgroundTemplateS(const QRect& rect);
	void pasteBackgroundTemplateS(const QRect& rect);

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
	int instrument_;
	QGraphicsRectItem* selectionRectItem_;
	bool selectingMode;
};