#include "ParametersWidget.h"

#include <QComboBox>

ParametersWidget::ParametersWidget(QWidget* parent ) :
	QWidget(parent)
{
	comboboxGridSize = new QComboBox(this);
}