#include "ParametersWidget.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>

void ParametersWidget::getGridSize(int& rows, int& cols)
{
	const int c_index = comboboxGridSize->currentIndex();
	if (c_index == GRID_SIZE_17x17)
	{
		rows = 17;
		cols = 17;
	}
	else if (c_index == GRID_SIZE_33x33)
	{
		rows = 33;
		cols = 33;
	}
}

ParametersWidget::ParametersWidget(QWidget* parent) :
	QWidget(parent)
{
	comboboxGridSize = new QComboBox(this);
	comboboxGridSize->insertItem(0, "17x17");
	comboboxGridSize->insertItem(1, "33x33");
	setFixedWidth(150);
	//setFixedHeight(200);

	QLabel* label = new QLabel(tr("Grid size:"), this);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(label);
	layout->addWidget(comboboxGridSize);
	layout->addStretch();

	QGroupBox* groupBox = new QGroupBox(tr("Parameters"), this);
	groupBox->setLayout(layout);

	connect(comboboxGridSize, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [&](int index) { updateParameters(); });
}

void ParametersWidget::updateParameters()
{
	Parameters params;
	getGridSize(params.gridRows, params.gridCols);
	emit parametersChangedS(params);
}

void ParametersWidget::setParameters(const Parameters& params)
{
	if (params.gridCols == 33 && params.gridRows == 33)
	{
		comboboxGridSize->setCurrentIndex(GRID_SIZE_33x33);
	}
	else if (params.gridCols == 17 && params.gridRows == 17)
	{
		comboboxGridSize->setCurrentIndex(GRID_SIZE_17x17);
	}
	else
	{
		Q_ASSERT(false);
	}
}

ParametersWidget::~ParametersWidget()
{

}