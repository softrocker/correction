#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

class QImage;
class QWidget;
class QString;

#include <opencv2/highgui/highgui.hpp>

namespace DataTransfer
{
	QImage loadImg(QWidget* parent);
	void saveImage(QWidget* parent, const QImage& image);
	QString imageName(QWidget* parent);
	void loadCvImage(QWidget* parent, cv::Mat& cvImage);
}

#endif