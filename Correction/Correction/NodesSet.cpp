#include "NodesSet.h"
#include "Parameters.h"

#define _USE_MATH_DEFINES // for C++  
#include <math.h>  

double angleBetween(const cv::Point &v1, const cv::Point &v2);
double angleRadiansToDegrees(double angleRadians);

std::vector<cv::Point> NodesSet::row(int index) const
{
	cv::Mat_<cv::Point> matRow = cv::Mat_<cv::Point>(nodes_, cv::Rect(0, index, nodes_.cols, 1));
	std::vector<cv::Point> v;
	v.reserve(nodes_.cols);
	for (int col = 0; col < nodes_.cols; col++)
	{
		v.push_back(matRow.at<cv::Point>(0, col));
	}
	return v;
}

std::vector<cv::Point> NodesSet::col(int index) const
{
	cv::Mat_<cv::Point> matCol = cv::Mat_<cv::Point>(nodes_, cv::Rect(index, 0, 1, nodes_.rows));
	std::vector<cv::Point> v;
	v.reserve(nodes_.rows);
	for (int row = 0; row < nodes_.rows; row++)
	{
		v.push_back(matCol.at<cv::Point>(row, 0));
	}
	return v;
}

void NodesSet::create(const std::vector<cv::Point>&nodes, int cols, int rows)
{
	nodes_.create(cv::Size(cols, rows));
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			nodes_.at<cv::Point>(row, col) = nodes[col + row * cols];
		}
	}
}

cv::Mat_<cv::Point> NodesSet::getNodes() const
{
	return nodes_;
}

const cv::Point& NodesSet::at(int row, int col) const
{
	return nodes_.at<cv::Point>(row, col);
}

cv::Point& NodesSet::at(int row, int col)
{
	return nodes_.at<cv::Point>(row, col);
}

const cv::Point& NodesSet::center() const
{
	return at(cols() / 2, rows() / 2);
}

cv::Size NodesSet::getCellSize() const
{
	assert(nodes_.cols >= 2 && nodes_.rows >= 2);
	cv::Point nodeCentral = center();
	cv::Point nodeCentralDiag = at(rows() / 2 + 1, cols() / 2 + 1);
	return cv::Size(abs(nodeCentralDiag.x - nodeCentral.x), abs(nodeCentralDiag.y - nodeCentral.y));
}

void NodesSet::setNode(int row, int col, const cv::Point& p)
{
	nodes_.at<cv::Point>(row, col) = p;
}

NodeType NodesSet::getNodeType(int row, int col)
{
	if ((row > 0) && (row < rows() - 1) && (col > 0) && (col < cols() - 1))
	{
		return NODETYPE_CENTER;
	}
	if (row == 0 && col == 0)
	{
		return NODETYPE_BOTTOM_LEFT;
	}
	else if ((row == 0) && (col > 0) && (col < cols() - 1))
	{
		return NODETYPE_BOTTOM;
	}
	else if ((row == 0) && (col == cols() - 1))
	{
		return NODETYPE_BOTTOM_RIGHT;
	}
	else if ((row == rows() - 1) && (col == 0))
	{
		return NODETYPE_TOP_LEFT;
	}
	else if ((row == rows() - 1) && (col > 0) && (col < cols() - 1))
	{
		return NODETYPE_TOP;
	}
	else if ((row == rows() - 1) && (col == cols() - 1))
	{
		return NODETYPE_TOP_RIGHT;
	}
	else if ((row > 0) && (row < rows() - 1) && (col == cols() - 1))
	{
		return NODETYPE_RIGHT;
	}
	else if ( (row > 0) && (row < rows() - 1) && (col == 0) )
	{
		return NODETYPE_LEFT;
	}
	else
	{
		assert(false);
		return NODETYPE_INVALID;
	}
}

bool NodesSet::empty()
{
	return nodes_.empty();
}

double NodesSet::getAngle(int row, int col, int dir) 
{
	assert(!empty());
	assert(row >= 0 && row < rows() && col >= 0 && col < cols());
	double angleRadians;
	cv::Point node = at(row, col);
	if (col < cols() - 1)
	{
		cv::Point nodeRight = at(row, col + 1);
		angleRadians = angleBetween(nodeRight, node);
	}
	else // col == cols() - 1
	{
		cv::Point nodeLeft = at(row, col - 1);
		angleRadians = angleBetween(node, nodeLeft);
	}
	if (dir == -1)
		return -angleRadians;
	return angleRadians;
}

int NodesSet::cols() const
{
	return nodes_.cols;
}

int NodesSet::rows() const
{
	return nodes_.rows;
}

double angleBetween(const cv::Point &p1, const cv::Point &p2)
{
	double dY = p2.y - p1.y;
	double dX = p2.x - p1.x;
	return atan2(dY, dX);
}

