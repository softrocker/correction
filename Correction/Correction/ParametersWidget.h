#pragma once

#include <QWidget>

class QComboBox;

class ParametersWidget : public QWidget
{
public:
	ParametersWidget(QWidget* parent = 0);
	~ParametersWidget();
private:
	
	QComboBox* comboboxGridSize;
};