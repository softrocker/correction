#pragma once
#include <vector>
#include <opencv2/core/core.hpp>

enum NodeType
{
	NODETYPE_INVALID = -1,
	NODETYPE_CENTER = 0,
	NODETYPE_TOP_LEFT = 1,
	NODETYPE_TOP_RIGHT = 2,
	NODETYPE_BOTTOM_LEFT = 3,
	NODETYPE_BOTTOM_RIGHT = 4,
	NODETYPE_LEFT = 5,
	NODETYPE_TOP = 6,
	NODETYPE_RIGHT = 7,
	NODETYPE_BOTTOM = 8
};

class NodesSet
{
public:
	cv::Point& at(int row, int col);
	const cv::Point& at(int row, int col) const;
	void create(const std::vector<cv::Point>& nodes, int cols, int rows);
	const cv::Point& center() const;
	bool empty();
	cv::Mat_<cv::Point> getNodes() const;
	cv::Size getCellSize() const;
	NodeType getNodeType(int row, int col);
	double getAngle(int row, int col, int dir); // returns approximate value of angle (in radians) of the cross (by approximately found nodes)
	void setNode(int row, int col, const cv::Point& p);

	std::vector<cv::Point> row(int index) const;
	std::vector<cv::Point> col(int index) const;
	int cols() const;
	int rows() const;
	
private:
	cv::Mat_<cv::Point> nodes_;
};