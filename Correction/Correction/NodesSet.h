#pragma once
#include <vector>
#include <opencv2/core/core.hpp>

enum NodeType
{
	CENTER = 0,
	TOP_LEFT = 1,
	TOP_RIGHT = 2,
	BOTTOM_LEFT = 3,
	BOTTOM_RIGHT = 4,
	LEFT = 5,
	TOP = 6,
	RIGHT = 7,
	BOTTOM = 8
};

class NodesSet
{
public:
	void create(const std::vector<cv::Point>& nodes, int cols, int rows);
	std::vector<cv::Point> row(int index) const;
	std::vector<cv::Point> col(int index) const;
	int cols() const;
	int rows() const;
	cv::Mat_<cv::Point> getNodes() const;
	const cv::Point& at(int row, int col) const;
	cv::Point& at(int row, int col);
	const cv::Point& center() const;
	cv::Size getCellSize() const;
	void setNode(int row, int col, const cv::Point& p);
	NodeType getNodeType(int row, int col);

private:
	cv::Mat_<cv::Point> nodes_;
};