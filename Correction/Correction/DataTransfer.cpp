#include "DataTransfer.h"
#include <QImage>
#include <QFileDialog>
#include <QWidget>
#include <QMessageBox>
#include <QImageReader>

#include <fstream>

#include <opencv2/highgui/highgui.hpp>

namespace DataTransfer
{
	QImage loadImg(QWidget* parent)
	{
		QString fileName = QFileDialog::getOpenFileName(parent, QObject::tr("Choose image for import"), "", QObject::tr("Images (*.png *.jpg *.bmp)"));
		if (!fileName.isEmpty())
		{
			return QImage(fileName);
		}
		return QImage();
	}

	QString imageName(QWidget* parent)
	{
		return QFileDialog::getOpenFileName(parent, QObject::tr("Choose image for import"), "", QObject::tr("Images (*.png *.jpg *.bmp)"));
	}

	void loadCvImage(QWidget* parent, cv::Mat& cvImage)
	{
		QString imgName = imageName(parent);
		if (imgName.isEmpty())
		{
			return;
		}
		cvImage = cv::imread(imgName.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
	}

	void saveImage(QWidget* parent, const QImage& image)
	{
		if (image.isNull())
		{
			QMessageBox::critical(parent, QObject::tr("Save image error"), QObject::tr("Unable to save image. Image has zero size."));
			return;
		}
		QString selectedFilter;
		QString fileName = QFileDialog::getSaveFileName(parent, QObject::tr("Save image as..."), "image.png", QObject::tr("Images (*.png *.jpg *.bmp)"), &selectedFilter);
		if (selectedFilter.contains("png") || selectedFilter.contains("jpg"))
		{
			image.save(fileName);
		}
		else
			QMessageBox::critical(parent, QObject::tr("Save image error"), QObject::tr("Unknown format. Please, choose another format."));

	}

	void saveValuesToFile(const std::vector<double>& values, const std::string& filename)
	{
		std::ofstream streamOut(filename);
		for (int i = 0; i < values.size(); i++)
		{
			streamOut << values[i] << std::endl;
		}
		streamOut.close();
	}
}