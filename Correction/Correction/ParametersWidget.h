#pragma once

#include <QWidget>

#include "Parameters.h"

class QComboBox;
class QDoubleSpinBox;

class ParametersWidget : public QWidget
{
	Q_OBJECT
public:
//	void getGridSize(int& rows, int& cols);
	ParametersWidget(QWidget* parent = 0);
	void updateParameters();
	void setParameters(const Parameters& params);
	Parameters getParameters();
	~ParametersWidget();
signals:
	void parametersChangedS(const Parameters& params);
private:
	QComboBox* comboboxGridSize_;
	QDoubleSpinBox* spinboxCellSizeFactor_;
};

enum GridSize
{
	GRID_SIZE_17x17 = 0,
	GRID_SIZE_33x33 = 1
};