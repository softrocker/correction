#include "ParametersWidget.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>

//void ParametersWidget::getGridSize(int& rows, int& cols)
//{
//	const int c_index = comboboxGridSize_->currentIndex();
//	if (c_index == GRID_SIZE_17x17)
//	{
//		rows = 17;
//		cols = 17;
//	}
//	else if (c_index == GRID_SIZE_33x33)
//	{
//		rows = 33;
//		cols = 33;
//	}
//}

ParametersWidget::ParametersWidget(QWidget* parent) :
	QWidget(parent)
{
	// main layout of the widget
	QVBoxLayout* layoutMain = new QVBoxLayout(); 

	comboboxGridSize_ = new QComboBox(this);
	comboboxGridSize_->insertItem(0, "17x17");
	comboboxGridSize_->insertItem(1, "33x33");
	setFixedWidth(200);
	QLabel* label = new QLabel(tr("Grid size:"), this);
	
	QHBoxLayout* layoutGridSize = new QHBoxLayout();
	layoutGridSize->addWidget(label);
	layoutGridSize->addWidget(comboboxGridSize_);

	layoutMain->addLayout(layoutGridSize);

	spinboxCellSizeFactor_ = new QDoubleSpinBox(this);
	spinboxCellSizeFactor_->setRange(0.1, 1.5);
	QLabel* labelCellSizeFactor = new QLabel(tr("Cell size factor:"), this);
	QHBoxLayout* layoutCellSizeFactor = new QHBoxLayout();
	layoutCellSizeFactor->addWidget(labelCellSizeFactor);
	layoutCellSizeFactor->addWidget(spinboxCellSizeFactor_);

	layoutMain->addLayout(layoutCellSizeFactor);

	spinboxBlurImage_ = new QSpinBox(this);
	spinboxBlurImage_->setRange(0, 30);
	QLabel* labelBlurImage = new QLabel(tr("Image blur (pixels):"), this);
	QHBoxLayout* layoutBlurImage = new QHBoxLayout();
	layoutBlurImage->addWidget(labelBlurImage);
	layoutBlurImage->addWidget(spinboxBlurImage_);

	layoutMain->addLayout(layoutBlurImage);

	spinboxBlurMask_ = new QSpinBox(this);
	spinboxBlurMask_->setRange(0, 30);
	QLabel* labelBlurMask = new QLabel(tr("Mask blur (pixels):"), this);
	QHBoxLayout* layoutBlurMask = new QHBoxLayout();
	layoutBlurMask->addWidget(labelBlurMask);
	layoutBlurMask->addWidget(spinboxBlurMask_);

	layoutMain->addLayout(layoutBlurMask);

	QGroupBox* groupBox = new QGroupBox(tr("Parameters"), this);
	groupBox->setLayout(layoutMain);

	connect(comboboxGridSize_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [&](int index) { updateParameters(); });
	connect(spinboxCellSizeFactor_, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [&](int index) { updateParameters(); });
	connect(spinboxBlurImage_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int index) { updateParameters(); });
	connect(spinboxBlurMask_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int index) { updateParameters(); });
}

void ParametersWidget::updateParameters()
{
	Parameters params = getParameters();
	emit parametersChangedS(params);
}

void ParametersWidget::setParameters(const Parameters& params)
{
	if (params.gridCols == 33 && params.gridRows == 33)
	{
		comboboxGridSize_->setCurrentIndex(GRID_SIZE_33x33);
	}
	else if (params.gridCols == 17 && params.gridRows == 17)
	{
		comboboxGridSize_->setCurrentIndex(GRID_SIZE_17x17);
	}
	else
	{
		Q_ASSERT(false);
	}
	spinboxCellSizeFactor_->setValue(params.cellSizeFactor);
	spinboxBlurImage_->setValue(params.blurImage);
	spinboxBlurMask_->setValue(params.blurMask);
}

Parameters ParametersWidget::getParameters()
{
	Parameters params;
	const int c_index = comboboxGridSize_->currentIndex();
	if (c_index == GRID_SIZE_17x17)
	{
		params.gridRows = 17;
		params.gridCols = 17;
	}
	else if (c_index == GRID_SIZE_33x33)
	{
		params.gridRows = 33;
		params.gridCols = 33;
	}
	params.cellSizeFactor = spinboxCellSizeFactor_->value();
	params.blurImage = spinboxBlurImage_->value();
	params.blurMask = spinboxBlurMask_->value();
	return params;
}

ParametersWidget::~ParametersWidget()
{

}