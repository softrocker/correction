#pragma once

#include <QWidget>

#include "Parameters.h"

class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;

class ParametersWidget : public QWidget
{
	Q_OBJECT
public:
//	void getGridSize(int& rows, int& cols);
	ParametersWidget(QWidget* parent = 0);
	void updateParameters();
	void setParameters(const Parameters& params);
	Parameters getParameters();
 void	blockAllSignals(bool block);
	~ParametersWidget();
signals:
	void parametersChangedS(const Parameters& params);
private:
	QComboBox* comboboxGridSize_;
	QDoubleSpinBox* spinboxCellSizeFactor_;
	QSpinBox* spinboxBlurImage_;
	QSpinBox* spinboxBlurMask_;
	QSpinBox* spinboxMaxPosError_;

	QSpinBox* spinboxPeakNeighborhoodGlobal_;
	QSpinBox* spinboxPeakNeighborhoodLocal_;

	QCheckBox* checkboxThresholdEnabled_;
	QSpinBox* spinboxThresholdValue_;
	QCheckBox* checkboxAutoThresholdEnabled_;

	QComboBox* comboboxSmoothingAlgorithm_;
};

enum GridSize
{
	GRID_SIZE_17x17 = 0,
	GRID_SIZE_33x33 = 1
};